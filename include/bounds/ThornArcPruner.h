#pragma once
#include "core/Types.h"
#include <vector>

struct PruningResult {
    MDSPInstance prunedInstance;
    std::vector<long long> thornArcs;
    bool applied = false;
};

class ThornArcPruner {
public:
    static PruningResult prune(const MDSPInstance& inst);
    static std::vector<long long> reconstructPoints(const std::vector<long long>& P_h,
                                                    const std::vector<long long>& Dh);
private:
    static bool isThornArc(long long d, const std::vector<long long>& remainingD);
};
