#pragma once
#include "core/Types.h"
#include <vector>

// -----------------------------------------------------------------------------
// Section 4.4: Large Neighborhood Search (LNS) for MDSP (Algorithm 4)
//
// Uses Algorithm 3 (Arithmetic Sequence Summation) for the construction step,
// then applies destroy-and-repair iterations to find a tight upper bound u.
// -----------------------------------------------------------------------------
struct LNSResult {
    std::vector<long long> bestPoints;
    int u = 0;
};

class LNSUpperBound {
public:
    static LNSResult solve(const MDSPInstance& inst, long long B, int iterations = 50);

private:
    static std::vector<long long> computeDelta(const std::vector<long long>& P);
    static std::vector<long long> multisetDifference(const std::vector<long long>& D,
                                                     const std::vector<long long>& deltaP);
    // Greedy repair: add points one by one until all distances are covered
    static void greedyRepair(std::vector<long long>& P, const MDSPInstance& inst, long long B);
};
