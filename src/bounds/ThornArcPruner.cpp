#include "bounds/ThornArcPruner.h"
#include <algorithm>
#include <vector>

PruningResult ThornArcPruner::prune(const MDSPInstance& inst) {
    PruningResult res;
    res.prunedInstance = inst;

    if (inst.D.empty()) return res;

    long long maxD = *std::max_element(inst.D.begin(), inst.D.end());
    if (inst.k() >= 1000 || maxD > 1000) {
        return res;
    }

    std::vector<long long> remaining = inst.D;
    std::vector<long long> thornArcs;

    bool changed = true;
    while (changed) {
        changed = false;
        for (size_t i = 0; i < remaining.size(); ++i) {
            long long targetD = remaining[i];
            std::vector<long long> others;
            others.reserve(remaining.size() - 1);
            for (size_t j = 0; j < remaining.size(); ++j) {
                if (i != j) others.push_back(remaining[j]);
            }

            if (isThornArc(targetD, others)) {
                thornArcs.push_back(targetD);
                remaining.erase(remaining.begin() + i);
                changed = true;
                res.applied = true;
                break;
            }
        }
    }

    res.prunedInstance.D = remaining;
    res.thornArcs = thornArcs;
    return res;
}

bool ThornArcPruner::isThornArc(long long targetD, const std::vector<long long>& remainingD) {
    if (remainingD.empty()) return true;

    long long B_val = 0;
    for (long long w : remainingD) B_val += std::abs(w);
    long long A_val = -B_val;

    if (targetD > B_val || targetD < A_val) return true;

    int offset = static_cast<int>(-A_val);
    int rangeSize = static_cast<int>(B_val - A_val + 1);

    std::vector<bool> dp(rangeSize, false);
    dp[offset] = true;

    for (long long w : remainingD) {
        std::vector<bool> nextDp = dp;
        for (int x = 0; x < rangeSize; ++x) {
            if (dp[x]) {
                int nextX1 = x + static_cast<int>(w);
                if (nextX1 >= 0 && nextX1 < rangeSize) nextDp[nextX1] = true;
                int nextX2 = x - static_cast<int>(w);
                if (nextX2 >= 0 && nextX2 < rangeSize) nextDp[nextX2] = true;
            }
        }
        dp = std::move(nextDp);
    }

    int targetIdx = static_cast<int>(targetD + offset);
    if (targetIdx >= 0 && targetIdx < rangeSize && dp[targetIdx]) {
        return false;
    }
    return true;
}

std::vector<long long> ThornArcPruner::reconstructPoints(const std::vector<long long>& P_h,
                                                         const std::vector<long long>& Dh) {
    if (P_h.empty()) return {};

    std::vector<long long> P = P_h;

    // Theorem 1: for each thorn arc d, we must add exactly one new point
    // such that it realizes distance d with some existing point.
    // Try all base points and both directions to avoid collisions.
    for (long long d : Dh) {
        bool added = false;
        for (long long base : P) {
            long long candidate = base + d;
            if (std::find(P.begin(), P.end(), candidate) == P.end()) {
                P.push_back(candidate);
                added = true;
                break;
            }
            candidate = base - d;
            if (std::find(P.begin(), P.end(), candidate) == P.end()) {
                P.push_back(candidate);
                added = true;
                break;
            }
        }
        if (!added) {
            // Last resort: add beyond the current range
            long long maxP = *std::max_element(P.begin(), P.end());
            P.push_back(maxP + d);
        }
    }

    std::sort(P.begin(), P.end());

    // Shift so minimum point is 0
    if (!P.empty() && P.front() != 0) {
        long long shift = P.front();
        for (auto& val : P) val -= shift;
    }
    return P;
}
