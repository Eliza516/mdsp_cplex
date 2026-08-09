#pragma once
#include "core/Types.h"
#include <vector>

// -----------------------------------------------------------------------------
// Section 4.3: Arithmetic Sequence Summation Upper Bound (Algorithm 3)
//
// Constructs a feasible distance superset P by decomposing D into arithmetic
// progressions. An AP with common difference c and length k can be covered
// by k+1 equally spaced points: {a, a+c, a+2c, ..., a+kc}.
//
// The algorithm greedily selects the AP covering the most uncovered distances,
// merges the generated point sets, and repeats until all distances are covered.
// Returns a valid point set P with |P| as an upper bound on |P*|.
// -----------------------------------------------------------------------------
struct ArithSeqResult {
    std::vector<long long> points;
    int u = 0;
};

class ArithSequenceUB {
public:
    static ArithSeqResult construct(const MDSPInstance& inst, long long B);

private:
    // Given a set of points P (sorted), compute Delta(P) as a sorted vector
    static std::vector<long long> computeDelta(const std::vector<long long>& P);

    // Count how many distances in uncoveredD are covered by adding pointSet
    // to existing points P
    static int coverageGain(const std::vector<long long>& P,
                            const std::vector<long long>& pointSet,
                            const std::vector<long long>& uncoveredD);
};
