#pragma once
#include "core/Types.h"
#include <vector>

class FeasModel {
public:
    FeasModel(const MDSPInstance& inst, int t, long long B, double timeLimitSeconds = 3600.0);

    MDSPSolution solve(bool verbose = false);

private:
    const MDSPInstance& instance_;
    int t_;
    long long B_;
    double timeLimit_;

    std::vector<long long> distinctValues_;
    std::vector<int> multiplicities_;
    void computeMultiplicities();
};
