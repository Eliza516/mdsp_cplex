#include "bounds/LNSUpperBound.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <set>

static std::map<long long, int> toCountMap(const std::vector<long long>& v) {
    std::map<long long, int> m;
    for (long long x : v) m[x]++;
    return m;
}

static bool countMapEmpty(const std::map<long long, int>& m) {
    for (const auto& kv : m)
        if (kv.second > 0) return false;
    return true;
}

static std::vector<long long> computeDelta(const std::vector<long long>& P) {
    std::vector<long long> delta;
    const size_t n = P.size();
    for (size_t i = 0; i < n; ++i)
        for (size_t j = i + 1; j < n; ++j)
            delta.push_back(std::abs(P[j] - P[i]));
    std::sort(delta.begin(), delta.end());
    return delta;
}

static int computeNdCount(const std::vector<long long>& P,
                          const std::map<long long, int>& dprime,
                          long long x)
{
    std::map<long long, int> newDists;
    for (long long a : P) {
        long long dist = std::abs(x - a);
        if (dist > 0) newDists[dist]++;
    }
    int count = 0;
    for (const auto& kv : newDists) {
        auto it = dprime.find(kv.first);
        if (it != dprime.end() && it->second > 0) {
            count += std::min(kv.second, it->second);
        }
    }
    return count;
}

static void removeNdFromDprime(const std::vector<long long>& P,
                               std::map<long long, int>& dprime,
                               long long x)
{
    std::map<long long, int> newDists;
    for (long long a : P) {
        long long dist = std::abs(x - a);
        if (dist > 0) newDists[dist]++;
    }
    for (const auto& kv : newDists) {
        auto it = dprime.find(kv.first);
        if (it != dprime.end() && it->second > 0) {
            int matched = std::min(kv.second, it->second);
            it->second -= matched;
            if (it->second <= 0) dprime.erase(it);
        }
    }
}

static std::vector<long long> lnsConstruct(
        const std::vector<long long>& P_init,
        const MDSPInstance& inst,
        long long B,
        std::mt19937& rng)
{
    std::vector<long long> P = P_init;
    std::set<long long> P_set(P.begin(), P.end());
    std::map<long long, int> dprime = toCountMap(inst.D);
    
    std::vector<long long> deltaP = computeDelta(P);
    for (long long d : deltaP) {
        auto it = dprime.find(d);
        if (it != dprime.end() && it->second > 0) {
            it->second--;
            if (it->second <= 0) dprime.erase(it);
        }
    }

    while (!countMapEmpty(dprime)) {
        std::set<long long> candidateSet;
        for (long long a : P) {
            for (const auto& kv : dprime) {
                if (kv.second <= 0) continue;
                long long d = kv.first;
                long long x1 = a + d;
                long long x2 = a - d;
                if (x1 >= 0 && x1 <= B && P_set.count(x1) == 0)
                    candidateSet.insert(x1);
                if (x2 >= 0 && x2 <= B && P_set.count(x2) == 0)
                    candidateSet.insert(x2);
            }
        }

        if (candidateSet.empty()) {
            // fallback if no exact matches (should theoretically only happen if B is too small, but handle gracefully)
            for (long long x = 0; x <= B; ++x) {
                if (P_set.count(x) == 0) candidateSet.insert(x);
            }
        }

        int bestCount = -1;
        std::vector<long long> bestCandidates;

        for (long long x : candidateSet) {
            int count = computeNdCount(P, dprime, x);
            if (count > bestCount) {
                bestCount = count;
                bestCandidates.clear();
                bestCandidates.push_back(x);
            } else if (count == bestCount) {
                bestCandidates.push_back(x);
            }
        }

        if (bestCount <= 0 && candidateSet.empty()) break; 
        if (bestCandidates.empty()) break;

        std::uniform_int_distribution<size_t> dist(0, bestCandidates.size() - 1);
        long long bestX = bestCandidates[dist(rng)];

        removeNdFromDprime(P, dprime, bestX);

        P.push_back(bestX);
        P_set.insert(bestX);
    }

    std::sort(P.begin(), P.end());
    return P;
}

static std::vector<long long> lnsDestroy(
        const std::vector<long long>& P_in,
        const MDSPInstance& inst,
        std::mt19937& rng)
{
    int b = static_cast<int>(std::floor(0.1 * static_cast<double>(P_in.size())));
    
    // Based on pseudocode:
    // b ← ⌊0.1 × |P|⌋
    // Q ← {0}
    // D' ← D
    std::vector<long long> Q = {0};
    std::set<long long> Qset = {0};
    std::map<long long, int> dprime = toCountMap(inst.D);
    
    std::vector<long long> P = P_in;
    P.erase(std::remove(P.begin(), P.end(), 0), P.end());

    while (static_cast<int>(Q.size()) < b && !P.empty()) {
        int bestCount = -1;
        std::vector<long long> bestCandidates;

        for (long long x : P) {
            int count = computeNdCount(P, dprime, x);
            if (count > bestCount) {
                bestCount = count;
                bestCandidates.clear();
                bestCandidates.push_back(x);
            } else if (count == bestCount) {
                bestCandidates.push_back(x);
            }
        }

        if (bestCandidates.empty()) break;

        std::uniform_int_distribution<size_t> dist(0, bestCandidates.size() - 1);
        long long bestX = bestCandidates[dist(rng)];

        Q.push_back(bestX);
        Qset.insert(bestX);

        std::vector<long long> P_without_x = P;
        P_without_x.erase(std::remove(P_without_x.begin(), P_without_x.end(), bestX), P_without_x.end());
        
        removeNdFromDprime(P_without_x, dprime, bestX);

        P = P_without_x;
    }

    std::sort(Q.begin(), Q.end());
    return Q;
}

LNSResult LNSUpperBound::solve(const MDSPInstance& inst, long long B, int iterations) {
    LNSResult res;
    if (inst.D.empty()) return res;

    std::mt19937 rng(42);

    std::vector<long long> P = {0};
    std::vector<long long> P_best;

    for (int iter = 0; iter < iterations; ++iter) {
        P = lnsConstruct(P, inst, B, rng);
        if (P_best.empty() || P.size() < P_best.size()) {
            P_best = P;
        }
        P = lnsDestroy(P, inst, rng);
    }

    std::sort(P_best.begin(), P_best.end());
    res.bestPoints = P_best;
    res.u = static_cast<int>(P_best.size());
    return res;
}
