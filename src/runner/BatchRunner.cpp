#include "runner/BatchRunner.h"
#include "core/Types.h"
#include "core/Reader.h"
#include "bounds/Bounds.h"
#include "bounds/ThornArcPruner.h"
#include "bounds/CyclicLowerBound.h"
#include "bounds/LNSUpperBound.h"
#include "bounds/ArithSequenceUB.h"
#include "models/Model.h"
#include "models/BinarySearchSolver.h"
#include "runner/CsvExporter.h"

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// START: resource-control additions
BatchRunner::BatchRunner(std::string csvOutputPath, double perInstanceTimeLimitSeconds,
                                                 bool verbose, std::string algo,
                                                 int threads, int workMemMB, int concurrency)
        : csvOutputPath_(std::move(csvOutputPath)),
            timeLimit_(perInstanceTimeLimitSeconds),
            verbose_(verbose),
            algo_(std::move(algo)),
            threads_(threads), workMemMB_(workMemMB), concurrency_(concurrency) {}
// END: resource-control additions

void BatchRunner::runDirectory(const std::string& dirPath) {
    std::vector<std::string> files;

    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "Could not open directory: " << dirPath << "\n";
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        std::string full = dirPath + "/" + name;

        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            files.push_back(full);
        }
    }
    closedir(dir);

    std::sort(files.begin(), files.end());

    // If concurrency_ <= 1, run sequentially. Otherwise split into `concurrency_` groups
    if (concurrency_ <= 1 || files.size() < 2) {
        runFiles(files);
        return;
    }

    int workers = std::min(static_cast<size_t>(concurrency_), files.size());
    std::vector<std::vector<std::string>> groups(workers);
    // divide contiguously so nearby files stay together
    size_t base = files.size() / workers;
    size_t rem = files.size() % workers;
    size_t idx = 0;
    for (int w = 0; w < workers; ++w) {
        size_t chunk = base + (w < static_cast<int>(rem) ? 1 : 0);
        for (size_t j = 0; j < chunk; ++j) {
            groups[w].push_back(files[idx++]);
        }
    }

    // spawn worker threads
    std::vector<std::thread> ths;
    for (int w = 0; w < workers; ++w) {
        ths.emplace_back([this, g = std::move(groups[w])]() mutable {
            this->runFiles(g);
        });
    }

    for (auto& t : ths) t.join();
}

void BatchRunner::runFiles(const std::vector<std::string>& paths) {
    CsvExporter csv(csvOutputPath_);

    int done = 0;
    for (const auto& path : paths) {
        auto slash = path.find_last_of('/');
        std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);

        if (csv.alreadyHas(name)) {
            std::cout << "[skip] " << name << " (already in " << csvOutputPath_ << ")\n";
            continue;
        }

        std::cout << "[" << (++done) << "/" << paths.size() << "] Solving " << name << " ... \n"
                   << std::flush;
        runOne(path);
    }
}

static RunResult executeConfig(const MDSPInstance& baseInst, const std::string& algo, bool usePruning, bool useCyclic, bool useLNS, double timeLimit, bool verbose,
                                int threads, int workMemMB) {
    RunResult res;
    if (algo == "max") {
        res.valid = false;
        return res; // MAX is not implemented
    }

    try {
        MDSPInstance currentInst = baseInst;
        std::vector<long long> thornArcs;

        if (usePruning) {
            PruningResult pruneRes = ThornArcPruner::prune(baseInst);
            if (pruneRes.applied) {
                currentInst = pruneRes.prunedInstance;
                thornArcs = pruneRes.thornArcs;
            }
        }

        MDSPBounds bounds = MDSPBoundsCalculator::computeTrivialBounds(currentInst);

        if (useCyclic) {
            int l_cyclic = CyclicLowerBound::computeLowerBound(currentInst);
            if (l_cyclic > bounds.l) bounds.l = l_cyclic;
        }

        if (useLNS) {
            ArithSeqResult arithRes = ArithSequenceUB::construct(currentInst, bounds.B);
            if (arithRes.u > 0 && arithRes.u < bounds.u) bounds.u = arithRes.u;

            LNSResult lnsRes = LNSUpperBound::solve(currentInst, bounds.B, 100);
            if (lnsRes.u > 0 && lnsRes.u < bounds.u) bounds.u = lnsRes.u;
        }

        bounds.B = MDSPBoundsCalculator::refineValueBound(currentInst, bounds.u);

        MDSPSolution sol;
        auto t0 = std::chrono::steady_clock::now();

        if (algo == "feas") {
            sol = BinarySearchSolver::solve(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, verbose, threads, workMemMB);
        } else {
            MDSPModel model(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, threads, workMemMB);
            sol = model.solve(/*verbose=*/verbose);
        }

        auto t1 = std::chrono::steady_clock::now();
        res.timeSeconds = std::chrono::duration<double>(t1 - t0).count();
        res.valid = true;

        if (!thornArcs.empty() && !sol.points.empty()) {
            sol.points = ThornArcPruner::reconstructPoints(sol.points, thornArcs);
            sol.objective = static_cast<double>(sol.points.size());
        }

        if (sol.feasible) {
            res.lb = sol.bestBound;
            res.ub = sol.objective;
            res.gapPercent = sol.gapPercent();
        } else {
            res.lb = sol.bestBound;
            res.ub = -1;
            res.gapPercent = 100.0;
        }
    } catch (...) {
        res.valid = false;
    }
    return res;
}

void BatchRunner::runOne(const std::string& path) {
    CsvExporter csv(csvOutputPath_);

    auto slash = path.find_last_of('/');
    std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);

    ResultRow row;
    row.instanceName = name;

    try {
        MDSPInstance inst = MDSPReader::readFromFile(path);
        row.k = inst.k();

        std::cout << "  nIP... " << std::flush;
        row.nIP   = executeConfig(inst, "p1",   false, true,  true,  timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "nFEAS... " << std::flush;
        row.nFEAS = executeConfig(inst, "feas", false, true,  true,  timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "nMAX... " << std::flush;
        row.nMAX  = executeConfig(inst, "max",  false, true,  true,  timeLimit_, verbose_, threads_, workMemMB_);

        std::cout << "\n  IP... " << std::flush;
        row.IP    = executeConfig(inst, "p1",   false, false, false, timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "FEAS... " << std::flush;
        row.FEAS  = executeConfig(inst, "feas", false, false, false, timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "MAX... " << std::flush;
        row.MAX   = executeConfig(inst, "max",  false, false, false, timeLimit_, verbose_, threads_, workMemMB_);

        std::cout << "\n  tIP... " << std::flush;
        row.tIP   = executeConfig(inst, "p1",   true,  true,  true,  timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "tFEAS... " << std::flush;
        row.tFEAS = executeConfig(inst, "feas", true,  true,  true,  timeLimit_, verbose_, threads_, workMemMB_);
        std::cout << "tMAX... \n";
        row.tMAX  = executeConfig(inst, "max",  true,  true,  true,  timeLimit_, verbose_, threads_, workMemMB_);

    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }

    csv.writeRow(row);
}
