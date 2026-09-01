// =============================================================================
// Thorn Arc Pruner — Section pre-processing
//
// Pseudo code reference: pseudo_code.md (PruningPhase)
//
// Hàm pruningPhase(D):
//   Nếu |D| ≥ 1000 HOẶC max(D) > 1000 → bỏ qua
//   remaining = bản sao D
//   Với mỗi d trong D:
//     others = remaining \ {d}
//     Nếu isThornArc(d, others): D_h.add(d); remaining.remove(d)
//   Trả về (remaining, D_h)
//
// Hàm isThornArc(d, othersD):
//   Subset-sum DP: Q[i][x] = Q[i-1][x] OR Q[i-1][x - othersD[i]]
//   Q[0][0] = True   (tổng tập rỗng = 0)
//   d là thorn arc ⟺ Q[n][d] == False
//
// Hàm reconstructFullSolution(P_h_star, D_h):
//   Chọn p cố định trong P_h_star
//   P* = P_h_star ∪ { d + p | d ∈ D_h }
// =============================================================================

#include "bounds/ThornArcPruner.h"
#include <algorithm>
#include <vector>

// ---------------------------------------------------------------------------
// isThornArc: kiểm tra d có phải thorn arc không
//
// d là thorn arc ⟺ KHÔNG tồn tại tổ hợp (subset sum) của othersD bằng d.
// DP: Q[i][x] = True nếu có subset {othersD[0..i-1]} tổng bằng x
//   Base: Q[0] = True  (subset rỗng, tổng = 0)
//   Transition: Q[i][x] = Q[i-1][x] OR Q[i-1][x - othersD[i-1]]
// ---------------------------------------------------------------------------
bool ThornArcPruner::isThornArc(long long d,
                                 const std::vector<long long>& othersD) {
    if (othersD.empty()) return true;

    // Cận trên/dưới tổng ±combination: B = Σ|w|, A = -B
    long long B = 0;
    for (long long w : othersD) B += std::abs(w);
    long long A = -B;

    // d nằm ngoài [A, B] → chắc chắn không đạt được
    if (d > B || d < A) return true;

    // DP với offset: dp[x + offset] = True nếu tổng x đạt được bằng ±combination
    // Transition: Q[i][x] = Q[i-1][x] OR Q[i-1][x - w] OR Q[i-1][x + w]
    int offset    = static_cast<int>(-A);           // shift về index dương
    int rangeSize = static_cast<int>(B - A) + 1;

    std::vector<bool> dp(rangeSize, false);
    dp[offset] = true;  // Q[0][0] = True (tổng 0 = tập rỗng)

    for (long long w : othersD) {
        std::vector<bool> next = dp;
        for (int x = 0; x < rangeSize; ++x) {
            if (!dp[x]) continue;
            int xp = x + static_cast<int>(w);
            int xm = x - static_cast<int>(w);
            if (xp >= 0 && xp < rangeSize) next[xp] = true;
            if (xm >= 0 && xm < rangeSize) next[xm] = true;
        }
        dp = std::move(next);
    }

    int targetIdx = static_cast<int>(d) + offset;
    if (targetIdx >= 0 && targetIdx < rangeSize && dp[targetIdx]) {
        return false;  // d đạt được bằng ±combination → KHÔNG phải thorn arc
    }
    return true;       // Không đạt được → LÀ thorn arc
}

// ---------------------------------------------------------------------------
// prune: pruningPhase(D)
//
// Duyệt một lần qua D. Với mỗi d, kiểm tra d có phải thorn arc trong
// remaining tại thời điểm đó không. Nếu có → loại d khỏi remaining.
// ---------------------------------------------------------------------------
PruningResult ThornArcPruner::prune(const MDSPInstance& inst) {
    PruningResult res;
    res.prunedInstance = inst;

    if (inst.D.empty()) return res;

    long long maxD = *std::max_element(inst.D.begin(), inst.D.end());
    // Chỉ áp dụng khi bài toán đủ nhỏ (subset sum là NP-complete)
    if (inst.k() >= 1000 || maxD > 1000) {
        return res;
    }

    std::vector<long long> remaining = inst.D;
    std::vector<long long> thornArcs;

    // Duyệt một lần qua D (theo pseudo code: không lặp lại nhiều vòng)
    // Với mỗi d trong D gốc:
    //   others = remaining \ {d}   (xóa 1 lần xuất hiện đầu tiên của d)
    //   Nếu isThornArc(d, others) → d là thorn arc
    for (long long d : inst.D) {
        // Tìm d trong remaining (xóa 1 occurrence)
        auto it = std::find(remaining.begin(), remaining.end(), d);
        if (it == remaining.end()) continue;  // d đã bị loại bởi lần trước

        // others = remaining \ {d}
        std::vector<long long> others;
        others.reserve(remaining.size() - 1);
        for (auto jt = remaining.begin(); jt != remaining.end(); ++jt) {
            if (jt != it) others.push_back(*jt);
        }

        if (isThornArc(d, others)) {
            thornArcs.push_back(d);
            remaining.erase(it);
            res.applied = true;
        }
    }

    res.prunedInstance.D = remaining;
    res.thornArcs = thornArcs;
    return res;
}

// ---------------------------------------------------------------------------
// reconstructPoints: reconstructFullSolution(P_h_star, D_h)
//
// Theorem 1: P* = P*_h ∪ { d + p | d ∈ D_h }
// với p là một điểm cố định (bất kỳ) trong P_h_star, dùng CHUNG cho mọi d.
// ---------------------------------------------------------------------------
std::vector<long long> ThornArcPruner::reconstructPoints(
        const std::vector<long long>& P_h,
        const std::vector<long long>& Dh) {
    if (P_h.empty()) return {};

    // Chọn p cố định: dùng phần tử đầu tiên của P_h (bất kỳ phần tử nào đều hợp lệ)
    long long p = P_h.front();

    std::vector<long long> P = P_h;

    // Thêm d + p cho mỗi thorn arc d ∈ D_h
    for (long long d : Dh) {
        P.push_back(d + p);
    }

    std::sort(P.begin(), P.end());

    // Shift về 0 nếu cần
    if (!P.empty() && P.front() != 0) {
        long long shift = P.front();
        for (auto& val : P) val -= shift;
    }

    return P;
}
