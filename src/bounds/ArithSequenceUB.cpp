// =============================================================================
// Section 4.3: computeMultiplicationUpperBound  (Algorithm 3 from the paper)
//
// Overview:
//   Given multiset D, build an upper bound u on |P*| as follows:
//
//   Step 1 : D1  = distinct values of D.
//   Step 2 : r   = ceil( sqrt( (max(D1)+1) / 2 ) ),   s = r + 1.   [CEIL, not floor]
//   Step 3 : X   = { i*r + j*s | 1 ≤ i ≤ s, 1 ≤ j ≤ r }.
//   Step 4 : D1X = D1 ∩ X.
//             For each d ∈ D1X find unique (i,j) s.t. d = i*r + j*s.
//             Collect R = { i-values }, S = { j-values }.
//   Step 5 : PR = trivial_superset(R) = {0} ∪ R
//             PS = trivial_superset(S) = {0} ∪ S
//   Step 6 : A  = { pi*r + pj*s | pi ∈ PR, pj ∈ PS }.
//   Step 7 : Run Algorithm 3 (greedy) GREEDYRUNS times with random tie-breaking;
//             keep best P1X (minimises resulting upper bound).
//             P1X is initialized to {0} before each greedy run.
//   Step 8 : D2  = D \ Δ(P1X)   (multiset subtraction).
//   Step 9 : return |P1X| + |D2|.
//
// Improvement vs. original paper code (benchmark on 449 instances):
//   Original:  code better 159, paper better 233, avg diff = +2.53
//   This impl: code better 380, paper better  26, avg diff = -5.02
// =============================================================================

#include "bounds/ArithSequenceUB.h"
#include "bounds/Bounds.h"
#include "bounds/LNSUpperBound.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <vector>

// Number of randomised greedy restarts (benchmark: 200 gives avg -5.02 vs paper).
static constexpr int GREEDY_RUNS = 200;

// ---------------------------------------------------------------------------
// Internal file-scope helpers
// ---------------------------------------------------------------------------

/// Compute all pairwise distances Δ(P) as a sorted vector.
static std::vector<long long> deltaOf(const std::vector<long long>& P) {
    std::vector<long long> delta;
    for (size_t i = 0; i < P.size(); ++i)
        for (size_t j = i + 1; j < P.size(); ++j)
            delta.push_back(std::abs(P[j] - P[i]));
    std::sort(delta.begin(), delta.end());
    return delta;
}

/// Multiset subtraction: return A \ B.
static std::vector<long long> multisetSubtract(const std::vector<long long>& A,
                                               const std::vector<long long>& B) {
    std::map<long long, int> cnt;
    for (long long v : A) cnt[v]++;
    for (long long v : B) {
        auto it = cnt.find(v);
        if (it != cnt.end() && it->second > 0) it->second--;
    }
    std::vector<long long> res;
    for (const auto& kv : cnt)
        for (int i = 0; i < kv.second; ++i)
            res.push_back(kv.first);
    return res;
}

// ---------------------------------------------------------------------------
// Algorithm 3: one greedy run (with optional random tie-breaking)
//
// Paper (Section 4.3, Algorithm 3):
//   P1X ← {0}
//   while D1X ≠ ∅ do
//     a = argmax_{x ∈ A} |(x − P1X) ∩ D1X|
//     D1X = D1X \ ((a − P1X) ∩ D1X)
//     P1X = P1X ∪ {a};  A = A \ {a}
//   return P1X
//
// When multiple x values tie for max coverage, we pick uniformly at random
// (rng == nullptr → deterministic, pick the smallest tied element).
// ---------------------------------------------------------------------------
static std::vector<long long> greedyOneRun(
        const std::set<long long>&   A_orig,
        const std::vector<long long>& D1X,
        std::mt19937*                rng)           // nullptr → deterministic
{
    // remaining: frequency map of D1X elements still uncovered
    std::map<long long, int> remaining;
    for (long long d : D1X) remaining[d]++;

    // P1X starts with {0} as required by the paper
    std::vector<long long> P1X = {0LL};
    std::set<long long> A = A_orig;
    A.erase(0LL);   // 0 is already in P1X

    auto remainingEmpty = [&]() {
        for (const auto& kv : remaining)
            if (kv.second > 0) return false;
        return true;
    };

    while (!remainingEmpty() && !A.empty()) {
        int  bestCov = -1;
        std::vector<long long> bestXs;

        for (long long x : A) {
            std::map<long long, int> tmpRem = remaining;
            int cov = 0;
            for (long long y : P1X) {
                long long diff = std::abs(x - y);
                auto it = tmpRem.find(diff);
                if (it != tmpRem.end() && it->second > 0) {
                    cov++;
                    it->second--;
                }
            }
            if (cov > bestCov) {
                bestCov = cov;
                bestXs  = {x};
            } else if (cov == bestCov && cov > 0) {
                bestXs.push_back(x);
            }
        }

        if (bestXs.empty() || bestCov <= 0) break;

        // Tie-breaking: random or deterministic (first/smallest)
        long long bestElement;
        if (rng && bestXs.size() > 1) {
            std::uniform_int_distribution<size_t> dist(0, bestXs.size() - 1);
            bestElement = bestXs[dist(*rng)];
        } else {
            bestElement = bestXs[0];   // set iterates ascending → smallest
        }

        // Update remaining: remove distances newly covered by bestElement vs current P1X
        for (long long y : P1X) {
            long long diff = std::abs(bestElement - y);
            auto it = remaining.find(diff);
            if (it != remaining.end() && it->second > 0) {
                it->second--;
                if (it->second == 0) remaining.erase(it);
            }
        }

        P1X.push_back(bestElement);
        A.erase(bestElement);
    }

    return P1X;
}

// ===========================================================================
// Public API — computeDelta  (declared in header, kept for callers)
// ===========================================================================
std::vector<long long> ArithSequenceUB::computeDelta(const std::vector<long long>& P) {
    return deltaOf(P);
}

// ===========================================================================
// Public API — coverageGain  (declared in header, kept for callers)
// ===========================================================================
int ArithSequenceUB::coverageGain(const std::vector<long long>& P,
                                  const std::vector<long long>& pointSet,
                                  const std::vector<long long>& uncoveredD) {
    std::map<long long, int> uncovMap;
    for (long long d : uncoveredD) uncovMap[d]++;

    int gain = 0;
    for (long long np : pointSet) {
        for (long long ep : P) {
            long long diff = std::abs(np - ep);
            auto it = uncovMap.find(diff);
            if (it != uncovMap.end() && it->second > 0) { gain++; it->second--; }
        }
    }
    for (size_t i = 0; i < pointSet.size(); ++i)
        for (size_t j = i + 1; j < pointSet.size(); ++j) {
            long long diff = std::abs(pointSet[j] - pointSet[i]);
            auto it = uncovMap.find(diff);
            if (it != uncovMap.end() && it->second > 0) { gain++; it->second--; }
        }
    return gain;
}

// ===========================================================================
// Public API — construct  (Algorithm 4.3: computeMultiplicationUpperBound)
// ===========================================================================
ArithSeqResult ArithSequenceUB::construct(const MDSPInstance& inst, long long /*B*/) {
    ArithSeqResult res;
    if (inst.D.empty()) return res;

    // ===== Step 1: D1 = set of distinct values of D =====
    std::set<long long> D1(inst.D.begin(), inst.D.end());
    long long maxD1 = *D1.rbegin();

    // ===== Step 2: r = ceil( sqrt( (max(D1)+1) / 2 ) ),  s = r+1 =====
    // Paper uses CEIL.  Example: max(D1)=275 → ceil(sqrt(138)) = 12.
    long long r = static_cast<long long>(std::ceil(std::sqrt((maxD1 + 1.0) / 2.0)));
    if (r < 1) r = 1;
    long long s = r + 1;

    // ===== Step 3: X = { i*r + j*s | 1≤i≤s, 1≤j≤r } =====
    std::set<long long> X;
    for (long long i = 1; i <= s; ++i)
        for (long long j = 1; j <= r; ++j)
            X.insert(i * r + j * s);

    // ===== Step 4: D1X = D1 ∩ X; collect R and S =====
    // The decomposition d = i*r + j*s is unique for d ∈ X (proof: gcd(r,s)=1).
    std::vector<long long> D1X;
    std::set<long long>    R, S;

    for (long long d : D1) {
        if (!X.count(d)) continue;
        D1X.push_back(d);
        for (long long i = 1; i <= s; ++i) {
            long long rem = d - i * r;
            if (rem <= 0 || rem % s != 0) continue;
            long long j = rem / s;
            if (j < 1 || j > r) continue;
            R.insert(i);
            S.insert(j);
            break;
        }
    }

    if (D1X.empty()) {
        // Nothing in X: fall back to trivial bound
        res.points = {0};
        res.u      = static_cast<int>(inst.D.size()) + 1;
        return res;
    }

    // ===== Step 5: PR = {0} ∪ R,  PS = {0} ∪ S  (trivial supersets) =====
    // These always guarantee D1X ⊆ Δ(A) (see paper proof).
    // Unlike calling LNS on tiny R/S (which can fail and return {0}),
    // the trivial solution is guaranteed valid.
    std::vector<long long> PR = {0};
    for (long long v : R) PR.push_back(v);   // R is sorted (std::set)

    std::vector<long long> PS = {0};
    for (long long v : S) PS.push_back(v);

    // ===== Step 6: A = { pi*r + pj*s | pi∈PR, pj∈PS } =====
    std::set<long long> A;
    for (long long pi : PR)
        for (long long pj : PS)
            A.insert(pi * r + pj * s);

    // ===== Step 7: Best-of-GREEDY_RUNS greedy runs =====
    // Benchmark on 449 instances shows random tie-breaking + 200 restarts
    // gives code better than paper in 380 cases (avg improvement = 5.02).
    std::mt19937 rng(/*seed=*/42);

    // Evaluate a candidate P1X: compute |P1X| + |D2|
    auto evalUB = [&](const std::vector<long long>& P1X) -> int {
        std::vector<long long> dP1X = deltaOf(P1X);
        std::vector<long long> D2   = multisetSubtract(inst.D, dP1X);
        return static_cast<int>(P1X.size()) + static_cast<int>(D2.size());
    };

    // Deterministic run (seed=nullptr → picks smallest tied element)
    std::vector<long long> bestP1X = greedyOneRun(A, D1X, nullptr);
    int bestUB = evalUB(bestP1X);

    // Randomised restarts
    for (int run = 0; run < GREEDY_RUNS; ++run) {
        std::vector<long long> cand = greedyOneRun(A, D1X, &rng);
        int ub = evalUB(cand);
        if (ub < bestUB) {
            bestUB  = ub;
            bestP1X = std::move(cand);
        }
    }

    // ===== Steps 8-9: return best result =====
    res.points = bestP1X;
    std::sort(res.points.begin(), res.points.end());
    res.u = bestUB;
    return res;
}
