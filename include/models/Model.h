#pragma once
#include "core/Types.h"
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Integer Programming formulation (P1) from Section 3 of the paper.
// -----------------------------------------------------------------------------
class MDSPModel {
public:
    MDSPModel(const MDSPInstance& inst, int l, int u, long long B,
              double timeLimitSeconds = 3600.0);

    MDSPSolution solve(bool verbose = true);

private:
    const MDSPInstance& instance_;
    int l_;
    int u_;
    long long B_;
    double timeLimit_;

    std::vector<long long> distinctValues_;
    std::vector<int> multiplicities_;
    void computeMultiplicities();
};
