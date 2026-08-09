#include "bounds/ArithSequenceUB.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <set>

std::vector<long long> ArithSequenceUB::computeDelta(const std::vector<long long>& P) {
    std::vector<long long> delta;
    for (size_t i = 0; i < P.size(); ++i) {
        for (size_t j = i + 1; j < P.size(); ++j) {
            delta.push_back(P[j] - P[i]);
        }
    }
    std::sort(delta.begin(), delta.end());
    return delta;
}

int ArithSequenceUB::coverageGain(const std::vector<long long>& P,
                                  const std::vector<long long>& pointSet,
                                  const std::vector<long long>& uncoveredD) {
    // Count how many uncovered distances are newly realized by adding pointSet to P
    std::map<long long, int> uncovMap;
    for (long long d : uncoveredD) uncovMap[d]++;

    int gain = 0;
    // New distances: between existing P and new points, plus among new points
    for (long long newPt : pointSet) {
        for (long long existPt : P) {
            long long diff = std::abs(newPt - existPt);
            auto it = uncovMap.find(diff);
            if (it != uncovMap.end() && it->second > 0) {
                gain++;
                it->second--;
            }
        }
    }
    // Distances among new points themselves
    for (size_t i = 0; i < pointSet.size(); ++i) {
        for (size_t j = i + 1; j < pointSet.size(); ++j) {
            long long diff = std::abs(pointSet[j] - pointSet[i]);
            auto it = uncovMap.find(diff);
            if (it != uncovMap.end() && it->second > 0) {
                gain++;
                it->second--;
            }
        }
    }
    return gain;
}

ArithSeqResult ArithSequenceUB::construct(const MDSPInstance& inst, long long B) {
    ArithSeqResult res;
    if (inst.D.empty()) return res;

    // Multiset of uncovered distances
    std::map<long long, int> uncoveredMap;
    for (long long d : inst.D) uncoveredMap[d]++;

    // Start with point 0
    std::set<long long> pointSet;
    pointSet.insert(0);

    // Covered distances tracking
    auto markCovered = [&](long long newPt) {
        for (long long p : pointSet) {
            if (p == newPt) continue;
            long long diff = std::abs(newPt - p);
            auto it = uncoveredMap.find(diff);
            if (it != uncoveredMap.end() && it->second > 0) {
                it->second--;
                if (it->second == 0) uncoveredMap.erase(it);
            }
        }
    };

    auto allCovered = [&]() -> bool {
        return uncoveredMap.empty();
    };

    while (!allCovered()) {
        // --- Arithmetic Progression Search ---
        // For each candidate common difference c, find the longest AP
        // starting from some existing point that covers uncovered distances.

        long long bestC = 0;
        long long bestBase = 0;
        int bestLen = 0;
        int bestGain = 0;

        // Collect distinct uncovered distances as candidate common differences
        std::vector<long long> candidateCs;
        for (const auto& kv : uncoveredMap) {
            if (kv.second > 0) candidateCs.push_back(kv.first);
        }

        // Also try GCDs of pairs of uncovered distances as common differences
        for (size_t i = 0; i < candidateCs.size() && i < 20; ++i) {
            for (size_t j = i + 1; j < candidateCs.size() && j < 20; ++j) {
                long long g = std::gcd(candidateCs[i], candidateCs[j]);
                if (g > 0) candidateCs.push_back(g);
            }
        }
        // Deduplicate
        std::sort(candidateCs.begin(), candidateCs.end());
        candidateCs.erase(std::unique(candidateCs.begin(), candidateCs.end()), candidateCs.end());

        for (long long c : candidateCs) {
            if (c <= 0) continue;

            // For each existing point as base, extend an AP with step c
            for (long long base : pointSet) {
                // Build AP: base, base+c, base+2c, ...
                // Count how many new points we'd add and how many uncovered distances we'd cover
                std::vector<long long> newPoints;
                std::map<long long, int> tempUncov = uncoveredMap;
                int gain = 0;

                for (int step = 1; step <= 50; ++step) {
                    long long pt = base + static_cast<long long>(step) * c;
                    if (pt > B) break;
                    if (pointSet.count(pt)) continue;

                    // Check coverage gain of adding this point
                    bool anyGain = false;
                    // Distances to existing points
                    for (long long p : pointSet) {
                        long long diff = std::abs(pt - p);
                        auto it = tempUncov.find(diff);
                        if (it != tempUncov.end() && it->second > 0) {
                            anyGain = true;
                            gain++;
                            it->second--;
                            if (it->second == 0) tempUncov.erase(it);
                        }
                    }
                    // Distances to previously added new points in this AP
                    for (long long np : newPoints) {
                        long long diff = std::abs(pt - np);
                        auto it = tempUncov.find(diff);
                        if (it != tempUncov.end() && it->second > 0) {
                            anyGain = true;
                            gain++;
                            it->second--;
                            if (it->second == 0) tempUncov.erase(it);
                        }
                    }

                    newPoints.push_back(pt);

                    if (!anyGain && step > 2) break; // No more useful extensions
                }

                int len = static_cast<int>(newPoints.size());
                if (len == 0) continue;

                // Score: gain per point added (efficiency)
                // Prefer APs that cover more distances per new point
                if (gain > bestGain || (gain == bestGain && len < bestLen)) {
                    bestGain = gain;
                    bestLen = len;
                    bestC = c;
                    bestBase = base;
                }
            }
        }

        if (bestGain > 0) {
            // Apply the best AP found
            for (int step = 1; step <= 50; ++step) {
                long long pt = bestBase + static_cast<long long>(step) * bestC;
                if (pt > B) break;
                if (pointSet.count(pt)) continue;

                markCovered(pt);
                pointSet.insert(pt);

                // Check if done after each point
                if (allCovered()) break;
            }
        } else {
            // Fallback: greedily add the point covering the most uncovered distances
            long long bestPt = -1;
            int maxGain = 0;

            for (const auto& kv : uncoveredMap) {
                if (kv.second <= 0) continue;
                long long d = kv.first;
                for (long long p : pointSet) {
                    for (long long candidate : {p + d, p - d}) {
                        if (candidate < 0 || candidate > B) continue;
                        if (pointSet.count(candidate)) continue;

                        int g = 0;
                        for (long long ep : pointSet) {
                            long long diff = std::abs(candidate - ep);
                            auto it2 = uncoveredMap.find(diff);
                            if (it2 != uncoveredMap.end() && it2->second > 0) g++;
                        }
                        if (g > maxGain) {
                            maxGain = g;
                            bestPt = candidate;
                        }
                    }
                }
            }

            if (bestPt >= 0) {
                markCovered(bestPt);
                pointSet.insert(bestPt);
            } else {
                // Absolute fallback: pick any uncovered distance
                long long d = uncoveredMap.begin()->first;
                long long pt = *pointSet.rbegin() + d;
                if (pt > B) pt = d;
                markCovered(pt);
                pointSet.insert(pt);
            }
        }
    }

    res.points.assign(pointSet.begin(), pointSet.end());
    std::sort(res.points.begin(), res.points.end());
    res.u = static_cast<int>(res.points.size());
    return res;
}
