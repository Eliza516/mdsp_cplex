#pragma once
#include "core/Types.h"

class BinarySearchSolver {
public:
    static MDSPSolution solve(const MDSPInstance& inst, int l, int u, long long B,
                              double totalTimeLimitSeconds = 3600.0, bool verbose = false);
};
