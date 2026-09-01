#pragma once
#include "core/Types.h"

class BinarySearchSolver {
public:
    // START: resource-control additions
    // Added optional `threads` and `workMemMB` parameters to control CPLEX resource usage.
    static MDSPSolution solve(const MDSPInstance& inst, int l, int u, long long B,
                              double totalTimeLimitSeconds = 3600.0, bool verbose = false,
                              int threads = 0, int workMemMB = 1024);
    // END: resource-control additions
};
