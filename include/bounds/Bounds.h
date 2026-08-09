#pragma once
#include "core/Types.h"

// -----------------------------------------------------------------------------
// Implements initial trivial bounds (Fontoura et al. 2018) and Theorem 3
// (Section 4.5) point-value bound B refinement.
// -----------------------------------------------------------------------------
class MDSPBoundsCalculator {
public:
    static MDSPBounds computeTrivialBounds(const MDSPInstance& inst);
    static long long refineValueBound(const MDSPInstance& inst, int u);
};
