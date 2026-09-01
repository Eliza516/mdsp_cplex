#pragma once
#include "core/Types.h"
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
// Integer Programming formulation (P1) from Section 3 of the paper.
// -----------------------------------------------------------------------------
class MDSPModel {
public:
    // START: resource-control additions
    // Added optional `threads` and `workMemMB` parameters to control CPLEX resource usage.
    MDSPModel(const MDSPInstance& inst, int l, int u, long long B,
              double timeLimitSeconds = 3600.0, int threads = 0, int workMemMB = 1024);
    // END: resource-control additions

    MDSPSolution solve(bool verbose = true);
    void setWarmStart(const std::vector<long long>& points);

private:
    const MDSPInstance& instance_;
    int l_;
    int u_;
    long long B_;
    double timeLimit_;
    int threads_ = 0;
    int workMemMB_ = 1024;

    std::vector<long long> warmStartPoints_;
    std::vector<long long> distinctValues_;
    std::vector<int> multiplicities_;
    void computeMultiplicities();
};
