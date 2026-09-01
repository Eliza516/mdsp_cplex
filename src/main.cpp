#include "core/Types.h"
#include "core/Reader.h"
#include "bounds/Bounds.h"
#include "bounds/ThornArcPruner.h"
#include "bounds/CyclicLowerBound.h"
#include "bounds/ArithSequenceUB.h"
#include "bounds/LNSUpperBound.h"
#include "models/Model.h"
#include "models/MaxModel.h"
#include "models/BinarySearchSolver.h"
#include "runner/BatchRunner.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

static void printUsage(const char* prog) {
    std::cerr
        << "Single-instance mode:\n"
        << "  " << prog << " <instance_file> [time_limit_seconds] [--algo p1|feas] [--no-pruning] [--no-lns] [--no-cyclic]\n"
        << "  Example: " << prog << " data/full-5-15-1.txt 3600\n\n"
        << "Batch mode (solves every file in a directory, streams results to CSV):\n"
        << "  " << prog << " --batch <instances_dir> --out <results.csv> [--time <seconds>] [--algo p1|feas] [--quiet]\n"
        << "  Example: " << prog << " --batch data/ --out results.csv --time 3600 --algo p1\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    if (std::string(argv[1]) == "--batch") {
        std::string dir, outCsv;
        double timeLimit = 3600.0;
        bool verbose = true;
        std::string algo = "p1";
        // resource-control defaults
        int threads = 0;
        int workMemMB = 1024;
        int concurrency = 1;

        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--out" && i + 1 < argc) {
                outCsv = argv[++i];
            } else if (arg == "--time" && i + 1 < argc) {
                timeLimit = std::stod(argv[++i]);
            } else if (arg == "--algo" && i + 1 < argc) {
                algo = argv[++i];
            } else if (arg == "--threads" && i + 1 < argc) {
                threads = std::stoi(argv[++i]);
            } else if (arg == "--workmem" && i + 1 < argc) {
                workMemMB = std::stoi(argv[++i]);
            } else if (arg == "--concurrency" && i + 1 < argc) {
                concurrency = std::stoi(argv[++i]);
            } else if (arg == "--quiet") {
                verbose = false;
            } else if (dir.empty()) {
                dir = arg;
            }
        }

        if (dir.empty() || outCsv.empty()) {
            std::cerr << "Batch mode requires an instances directory and --out <results.csv>.\n\n";
            printUsage(argv[0]);
            return 1;
        }

        BatchRunner runner(outCsv, timeLimit, verbose, algo, threads, workMemMB, concurrency);
        runner.runDirectory(dir);
        return 0;
    }

    const std::string path = argv[1];
    double timeLimit = 3600.0;
    std::string algo = "p1";
    bool enablePruning = true;
    bool enableCyclic = true;
    bool enableLNS = true;
    // resource-control defaults for single-instance mode
    int threads = 0;
    int workMemMB = 1024;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--algo" && i + 1 < argc) {
            algo = argv[++i];
        } else if (arg == "--no-pruning") {
            enablePruning = false;
        } else if (arg == "--no-cyclic") {
            enableCyclic = false;
        } else if (arg == "--no-lns") {
            enableLNS = false;
        } else if (arg == "--threads" && i + 1 < argc) {
            // single-instance mode resource control
            // WARNING: keep to small values on personal machines
            threads = std::stoi(argv[++i]);
        } else if (i == 2 && arg.find_first_not_of("0123456789.") == std::string::npos) {
            timeLimit = std::stod(arg);
        }
    }

    try {
        MDSPInstance originalInst = MDSPReader::readFromFile(path);
        std::cout << "Loaded instance '" << path << "' with k = " << originalInst.k()
                  << " distances.\n";

        auto t0 = std::chrono::steady_clock::now();
        MDSPInstance currentInst = originalInst;
        std::vector<long long> thornArcs;

        if (enablePruning) {
            PruningResult pruneRes = ThornArcPruner::prune(originalInst);
            if (pruneRes.applied) {
                currentInst = pruneRes.prunedInstance;
                thornArcs = pruneRes.thornArcs;
                std::cout << "[Pre-processing] Thorn-arc pruning removed " << thornArcs.size()
                          << " distance(s). Remaining k = " << currentInst.k() << "\n";
            } else {
                std::cout << "[Pre-processing] Thorn-arc pruning removed 0 distance(s).\n";
            }
        }

        MDSPBounds bounds = MDSPBoundsCalculator::computeTrivialBounds(currentInst);
        std::cout << "[Pre-processing] Trivial bounds: l0 = " << bounds.l << ", u0 = " << bounds.u << "\n";

        if (enableCyclic) {
            int l_cyclic = CyclicLowerBound::computeLowerBound(currentInst);
            std::cout << "[Pre-processing] Cyclic Z/gZ lower bound result: l = " << l_cyclic << "\n";
            if (l_cyclic > bounds.l) {
                std::cout << "                 -> Improved global lower bound (l) from "
                          << bounds.l << " to " << l_cyclic << "\n";
                bounds.l = l_cyclic;
            }
        }

        std::vector<long long> lnsBestPoints;

        if (enableLNS) {
            // Algorithm 3: Arithmetic Sequence Summation (Section 4.3)
            ArithSeqResult arithRes = ArithSequenceUB::construct(currentInst, bounds.B);
            std::cout << "[Pre-processing] Arith-Seq construction (Alg 3) result: u = " << arithRes.u << "\n";
            if (arithRes.u > 0 && arithRes.u < bounds.u) {
                std::cout << "                 -> Improved global upper bound (u) from "
                          << bounds.u << " to " << arithRes.u << "\n";
                bounds.u = arithRes.u;
            }

            // Algorithm 4: LNS Destroy-and-Repair (Section 4.4)
            LNSResult lnsRes = LNSUpperBound::solve(currentInst, bounds.B, 100);
            std::cout << "[Pre-processing] LNS (Alg 4) result: u = " << lnsRes.u << "\n";
            if (lnsRes.u > 0 && lnsRes.u < bounds.u) {
                std::cout << "                 -> Improved global upper bound (u) from "
                          << bounds.u << " to " << lnsRes.u << "\n";
                bounds.u = lnsRes.u;
            }
            lnsBestPoints = lnsRes.bestPoints;
        }


        bounds.B = MDSPBoundsCalculator::refineValueBound(currentInst, bounds.u);

        auto t_prep = std::chrono::steady_clock::now();
        double prepTime = std::chrono::duration<double>(t_prep - t0).count();

        std::cout << "Final pre-processing bounds (Prep time: " << prepTime << " s):\n"
                  << "  l (lower bound |P*|) = " << bounds.l << "\n"
                  << "  u (upper bound |P*|) = " << bounds.u << "\n"
                  << "  B (max point value)  = " << bounds.B << "\n\n";

        MDSPSolution sol;

        if (algo == "feas") {
            std::cout << "Solving using Binary Search Algorithm (FEAS - Algorithm 1) ...\n";
            sol = BinarySearchSolver::solve(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, true, threads, workMemMB);
        } else if (algo == "max") {
            std::cout << "Solving using Max Model (Pseudo-Polynomial Formulation) ...\n";
            MDSPMaxModel model(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, threads, workMemMB);
            sol = model.solve(/*verbose=*/true);
        } else {
            std::cout << "Solving using Integer Programming Formulation (P1) ...\n";
            MDSPModel model(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, threads, workMemMB);
            if (!lnsBestPoints.empty()) model.setWarmStart(lnsBestPoints);
            sol = model.solve(/*verbose=*/true);
        }

        auto t1 = std::chrono::steady_clock::now();
        const double wall = std::chrono::duration<double>(t1 - t0).count();

        std::cout << "\n=================== RESULT ===================\n";
        if (!sol.feasible) {
            std::cout << "No feasible solution found within the time limit.\n";
            return 2;
        }

        if (!thornArcs.empty() && !sol.points.empty()) {
            sol.points = ThornArcPruner::reconstructPoints(sol.points, thornArcs);
            sol.objective = static_cast<double>(sol.points.size());
        }

        std::cout << (sol.optimal ? "Status   : OPTIMAL\n" : "Status   : FEASIBLE (time limit reached)\n");
        std::cout << "LB       = " << sol.bestBound << "\n";
        std::cout << "UB (|P*|)= " << static_cast<int>(sol.objective + 0.5) << "\n";
        std::cout << "Gap      = " << sol.gapPercent() << " %\n";
        std::cout << "P*       = { ";
        for (size_t i = 0; i < sol.points.size(); ++i) {
            std::cout << sol.points[i] << (i + 1 < sol.points.size() ? ", " : "");
        }
        std::cout << " }\n";
        std::cout << "CPLEX time : " << sol.cplexTimeSeconds << " s\n";
        std::cout << "Wall time  : " << wall << " s\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
