#include "runner/BatchRunner.h"
#include "core/Types.h"
#include "core/Reader.h"
#include "bounds/Bounds.h"
#include "bounds/ThornArcPruner.h"
#include "bounds/CyclicLowerBound.h"
#include "bounds/LNSUpperBound.h"
#include "models/Model.h"
#include "models/BinarySearchSolver.h"
#include "runner/CsvExporter.h"

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <iostream>

BatchRunner::BatchRunner(std::string csvOutputPath, double perInstanceTimeLimitSeconds,
                         bool verbose, std::string algo)
    : csvOutputPath_(std::move(csvOutputPath)),
      timeLimit_(perInstanceTimeLimitSeconds),
      verbose_(verbose),
      algo_(std::move(algo)) {}

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
    runFiles(files);
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

        std::cout << "[" << (++done) << "/" << paths.size() << "] Solving " << name << " ... "
                   << std::flush;
        runOne(path);
    }
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

        MDSPInstance currentInst = inst;
        std::vector<long long> thornArcs;

        PruningResult pruneRes = ThornArcPruner::prune(inst);
        if (pruneRes.applied) {
            currentInst = pruneRes.prunedInstance;
            thornArcs = pruneRes.thornArcs;
        }

        MDSPBounds bounds = MDSPBoundsCalculator::computeTrivialBounds(currentInst);

        int l_cyclic = CyclicLowerBound::computeLowerBound(currentInst);
        if (l_cyclic > bounds.l) bounds.l = l_cyclic;

        LNSResult lnsRes = LNSUpperBound::solve(currentInst, bounds.B, 100);
        if (lnsRes.u > 0 && lnsRes.u < bounds.u) bounds.u = lnsRes.u;

        bounds.B = MDSPBoundsCalculator::refineValueBound(currentInst, bounds.u);

        row.l = bounds.l;
        row.u = bounds.u;
        row.B = bounds.B;

        MDSPSolution sol;
        auto t0 = std::chrono::steady_clock::now();

        if (algo_ == "feas") {
            sol = BinarySearchSolver::solve(currentInst, bounds.l, bounds.u, bounds.B, timeLimit_, verbose_);
        } else {
            MDSPModel model(currentInst, bounds.l, bounds.u, bounds.B, timeLimit_);
            sol = model.solve(/*verbose=*/verbose_);
        }

        auto t1 = std::chrono::steady_clock::now();
        row.timeSeconds = std::chrono::duration<double>(t1 - t0).count();

        if (!thornArcs.empty() && !sol.points.empty()) {
            sol.points = ThornArcPruner::reconstructPoints(sol.points, thornArcs);
            sol.objective = static_cast<double>(sol.points.size());
        }

        if (sol.feasible) {
            row.lb = sol.bestBound;
            row.ub = sol.objective;
            row.gapPercent = sol.gapPercent();
            row.status = sol.optimal ? "OPTIMAL" : "FEASIBLE";
            std::cout << row.status << " |P*|=" << static_cast<int>(sol.objective + 0.5)
                      << " gap=" << row.gapPercent << "% time=" << row.timeSeconds << "s\n";
        } else {
            row.lb = sol.bestBound;
            row.ub = -1;
            row.gapPercent = 100.0;
            row.status = "NO_SOLUTION";
            std::cout << "NO SOLUTION within time limit (" << row.timeSeconds << "s)\n";
        }
    } catch (const std::exception& e) {
        row.status = std::string("ERROR: ") + e.what();
        std::cout << "ERROR: " << e.what() << "\n";
    }

    csv.writeRow(row);
}
