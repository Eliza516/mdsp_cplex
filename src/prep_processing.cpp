#include "core/Reader.h"
#include "bounds/Bounds.h"
#include "bounds/ThornArcPruner.h"
#include "bounds/CyclicLowerBound.h"
#include "bounds/LNSUpperBound.h"
#include "bounds/ArithSequenceUB.h"

#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;
using namespace std::chrono;

struct ReportRow {
    string name;
    int size;
    int tLB;
    int tUB;
    int HD;
    double timeHD;
    int nLB;
    double timeLB;
    int nUB;
    double timeUB;
    int nDUB;
    double timeDUB;
};

int main(int argc, char** argv) {
    string dirPath = "instances";
    if (argc > 1) dirPath = argv[1];

    vector<string> files;
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) return 1;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;
        string full = dirPath + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
            files.push_back(full);
        }
    }
    closedir(dir);

    sort(files.begin(), files.end(), [](const string& a, const string& b) {
        if (a.length() != b.length()) return a.length() < b.length();
        return a < b;
    });

    cout << "| Instances | Size | tLB | tUB | HD | HD Time (ms) | nLB | LB Time (ms) | nUB | UB Time (ms) | nDUB | DUB Time (ms) |\n";
    cout << "|---|---|---|---|---|---|---|---|---|---|---|---|\n";

    for (const auto& path : files) {
        auto slash = path.find_last_of('/');
        string name = (slash == string::npos) ? path : path.substr(slash + 1);

        try {
            MDSPInstance inst = MDSPReader::readFromFile(path);
            ReportRow row;
            row.name = name;
            row.size = inst.k();

            // HD Phase
            auto t0 = steady_clock::now();
            PruningResult pruneRes = ThornArcPruner::prune(inst);
            auto t1 = steady_clock::now();
            row.HD = pruneRes.thornArcs.size();
            row.timeHD = duration<double, std::milli>(t1 - t0).count();
            
            MDSPInstance currentInst = pruneRes.applied ? pruneRes.prunedInstance : inst;

            // Trivial Phase
            // Calculate trivial bounds on ORIGINAL instance for reporting
            MDSPBounds origBounds = MDSPBoundsCalculator::computeTrivialBounds(inst);
            row.tLB = origBounds.l;
            row.tUB = origBounds.u;

            // Calculate trivial bounds on PRUNED instance for algorithm logic
            MDSPBounds bounds = MDSPBoundsCalculator::computeTrivialBounds(currentInst);

            // LB Phase
            auto t2 = steady_clock::now();
            int l_cyclic = CyclicLowerBound::computeLowerBound(currentInst);
            auto t3 = steady_clock::now();
            int pruned_nLB = (l_cyclic > bounds.l) ? l_cyclic : bounds.l;
            row.nLB = pruned_nLB + row.HD;
            row.timeLB = duration<double, std::milli>(t3 - t2).count();

            // UB Phase
            auto t4 = steady_clock::now();
            ArithSeqResult arithRes = ArithSequenceUB::construct(currentInst, bounds.B);
            auto t5 = steady_clock::now();
            int pruned_nUB = (arithRes.u > 0 && arithRes.u < bounds.u) ? arithRes.u : bounds.u;
            row.nUB = pruned_nUB + row.HD;
            row.timeUB = duration<double, std::milli>(t5 - t4).count();

            // DUB Phase
            int current_ub = pruned_nUB; // start LNS from current best upper bound
            bounds.B = MDSPBoundsCalculator::refineValueBound(currentInst, current_ub);
            
            auto t6 = steady_clock::now();
            LNSResult lnsRes = LNSUpperBound::solve(currentInst, bounds.B, 100);
            auto t7 = steady_clock::now();
            int pruned_nDUB = (lnsRes.u > 0 && lnsRes.u < current_ub) ? lnsRes.u : current_ub;
            row.nDUB = pruned_nDUB + row.HD;
            row.timeDUB = duration<double, std::milli>(t7 - t6).count();

            cout << "| " << row.name << " | " << row.size << " | " 
                 << row.tLB << " | " << row.tUB << " | "
                 << row.HD << " | " << fixed << setprecision(1) << row.timeHD << " | "
                 << row.nLB << " | " << fixed << setprecision(1) << row.timeLB << " | "
                 << row.nUB << " | " << fixed << setprecision(1) << row.timeUB << " | "
                 << row.nDUB << " | " << fixed << setprecision(1) << row.timeDUB << " |\n";
                 
        } catch (...) {
            cout << "| " << name << " | ERROR |\n";
        }
    }
    return 0;
}
