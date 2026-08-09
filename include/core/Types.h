#pragma once
#include <vector>
#include <cstdint>

// -----------------------------------------------------------------------------
// A distance multiset instance for the Minimum Distance Superset Problem (MDSP).
// Given D = {d1, ..., dk}, we must find the minimum-size point set P such that
// D is a sub-multiset of Delta(P) = { pi - pj | pi, pj in P, pi > pj }.
// -----------------------------------------------------------------------------
struct MDSPInstance {
    std::vector<long long> D;   // the multiset of distances, as given in input
    int k() const { return static_cast<int>(D.size()); }
};

// -----------------------------------------------------------------------------
// Bounds computed during pre-processing:
//   l : lower bound on |P*|
//   u : upper bound on |P*|
//   B : upper bound on the value of any point p_i in an optimal solution
// -----------------------------------------------------------------------------
struct MDSPBounds {
    int l = 0;
    int u = 0;
    long long B = 0;
};

// -----------------------------------------------------------------------------
// Result of solving the MDSP model.
// -----------------------------------------------------------------------------
struct MDSPSolution {
    bool feasible = false;
    bool optimal  = false;
    bool timeLimitReached = false;
    double objective  = -1.0;       // UB: |P*|, size of the best incumbent found
    double bestBound   = -1.0;      // LB: CPLEX's best proven bound on |P*|
    std::vector<long long> points;  // the points of the retrieved distance superset
    double cplexTimeSeconds = 0.0;

    double gapPercent() const {
        if (objective <= 0) return 100.0;
        return 100.0 * (objective - bestBound) / objective;
    }
};
