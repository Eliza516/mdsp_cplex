#include "bounds/Bounds.h"
#include <algorithm>
#include <cmath>

MDSPBounds MDSPBoundsCalculator::computeTrivialBounds(const MDSPInstance& inst) {
    MDSPBounds b;
    const int k = inst.k();

    // l0 = ceil( 1/2 + sqrt(1/4 + 2k) )
    double raw = 0.5 + std::sqrt(0.25 + 2.0 * static_cast<double>(k));
    b.l = static_cast<int>(std::ceil(raw - 1e-9));

    // u0 = k + 1
    b.u = k + 1;

    // Tighten B with Theorem 3
    b.B = refineValueBound(inst, b.u);
    return b;
}

long long MDSPBoundsCalculator::refineValueBound(const MDSPInstance& inst, int u) {
    if (inst.D.empty()) return 0;

    std::vector<long long> sorted(inst.D);
    std::sort(sorted.begin(), sorted.end());

    const long long dmin = sorted.front();
    const long long dmax = sorted.back();

    const int n = static_cast<int>(sorted.size());
    const int uu = std::min(u, n);

    long long sumDu = 0;
    for (int i = n - uu; i < n; ++i) sumDu += sorted[i];

    long long candidate = (sumDu - dmin) / 2;
    return std::max(candidate, dmax);
}
