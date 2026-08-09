#include "bounds/LNSUpperBound.h"
#include "bounds/ArithSequenceUB.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>

std::vector<long long> LNSUpperBound::computeDelta(const std::vector<long long>& P) {
    std::vector<long long> delta;
    const size_t n = P.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            delta.push_back(std::abs(P[j] - P[i]));
        }
    }
    std::sort(delta.begin(), delta.end());
    return delta;
}

std::vector<long long> LNSUpperBound::multisetDifference(const std::vector<long long>& D,
                                                         const std::vector<long long>& deltaP) {
    std::map<long long, int> countsD;
    for (long long d : D) countsD[d]++;

    for (long long d : deltaP) {
        auto it = countsD.find(d);
        if (it != countsD.end() && it->second > 0) {
            it->second--;
        }
    }

    std::vector<long long> diff;
    for (const auto& kv : countsD) {
        for (int i = 0; i < kv.second; ++i) {
            diff.push_back(kv.first);
        }
    }
    return diff;
}

void LNSUpperBound::greedyRepair(std::vector<long long>& P, const MDSPInstance& inst, long long B) {
    // Repair phase: greedily add points to cover all remaining uncovered distances
    while (true) {
        std::vector<long long> deltaP = computeDelta(P);
        std::vector<long long> uncoveredD = multisetDifference(inst.D, deltaP);

        if (uncoveredD.empty()) break;

        std::map<long long, int> uncoveredMap;
        for (long long d : uncoveredD) uncoveredMap[d]++;

        // Generate candidate points: p +/- d for all p in P, d in uncoveredD
        std::vector<long long> candidates;
        for (long long p : P) {
            for (long long d : uncoveredD) {
                if (p + d <= B) candidates.push_back(p + d);
                if (p - d >= 0) candidates.push_back(p - d);
            }
        }
        std::sort(candidates.begin(), candidates.end());
        candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

        long long bestX = -1;
        int maxGain = -1;

        for (long long x : candidates) {
            if (std::find(P.begin(), P.end(), x) != P.end()) continue;

            int gain = 0;
            std::map<long long, int> tempMap = uncoveredMap;
            for (long long p : P) {
                long long dist = std::abs(x - p);
                auto it = tempMap.find(dist);
                if (it != tempMap.end() && it->second > 0) {
                    gain++;
                    it->second--;
                }
            }

            if (gain > maxGain) {
                maxGain = gain;
                bestX = x;
            }
        }

        if (bestX == -1 || maxGain <= 0) {
            bestX = P.back() + uncoveredD.front();
            if (bestX > B) bestX = B;
        }

        P.push_back(bestX);
    }
}

LNSResult LNSUpperBound::solve(const MDSPInstance& inst, long long B, int iterations) {
    LNSResult res;
    if (inst.D.empty()) return res;

    // --- Initial Construction using Algorithm 3 (Arithmetic Sequence Summation) ---
    ArithSeqResult initSol = ArithSequenceUB::construct(inst, B);
    std::vector<long long> bestP = initSol.points;

    std::mt19937 rng(42);

    // --- Destroy-and-Repair Loop (Algorithm 4) ---
    for (int iter = 0; iter < iterations; ++iter) {
        if (bestP.size() <= 2) break;

        // Destruction: KEEP b = 0.1 * |P| elements, remove the rest
        std::vector<long long> currentP = bestP;
        size_t b = std::max<size_t>(1, static_cast<size_t>(0.1 * currentP.size()));

        // Keep origin (0) always, shuffle the rest and keep (b-1) of them.
        auto it = std::find(currentP.begin(), currentP.end(), 0);
        if (it != currentP.end()) {
            std::swap(*it, currentP.front());
        }
        std::shuffle(currentP.begin() + 1, currentP.end(), rng);
        if (currentP.size() > b) {
            currentP.resize(b);
        }
        std::sort(currentP.begin(), currentP.end());

        // Repair: greedily add points to cover all uncovered distances
        greedyRepair(currentP, inst, B);

        if (currentP.size() < bestP.size()) {
            bestP = currentP;
        }
    }

    std::sort(bestP.begin(), bestP.end());
    res.bestPoints = bestP;
    res.u = static_cast<int>(bestP.size());
    return res;
}
