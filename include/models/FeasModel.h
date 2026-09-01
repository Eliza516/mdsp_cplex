#pragma once
#include "core/Types.h"
#include <vector>

class FeasModel {
public:
    // START: resource-control additions
    // Added optional `threads` and `workMemMB` parameters to control CPLEX resource usage.
    FeasModel(const MDSPInstance& inst, int t, long long B, double timeLimitSeconds = 3600.0,
              int threads = 0, int workMemMB = 1024);
    // END: resource-control additions

    MDSPSolution solve(bool verbose = false);

private:
    const MDSPInstance& instance_;
    int t_;
    long long B_;
    double timeLimit_;
    int threads_ = 0;
    int workMemMB_ = 1024;

    std::vector<long long> distinctValues_;
    std::vector<int> multiplicities_;
    void computeMultiplicities();
};
