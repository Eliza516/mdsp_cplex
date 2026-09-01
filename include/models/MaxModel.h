#ifndef MAX_MODEL_H
#define MAX_MODEL_H

#include "core/Types.h"
#include <vector>

// MaxCoverageModel: Given t points, maximize the number of distances covered.
// Used in binary search: if max coverage == |D|, then t is feasible.
// This matches the paper's MAX variant (assignment-based, not pseudo-polynomial).
class MDSPMaxModel {
public:
    MDSPMaxModel(const MDSPInstance& inst, int l, int u, long long B, double timeLimitSeconds, int threads = 0, int workMemMB = 1024);

    // Solve as max-coverage for a given target t.
    // If t == -1 (default), uses the stored u_ as target.
    MDSPSolution solve(bool verbose = false);
    MDSPSolution solveForT(int t, double timeLimitSeconds, bool verbose = false);

private:
    void computeMultiplicities();

    MDSPInstance instance_;
    int l_;
    int u_;
    long long B_;
    double timeLimit_;
    int threads_;
    int workMemMB_;

    std::vector<long long> distinctValues_;
    std::vector<int> multiplicities_;
};

#endif // MAX_MODEL_H
