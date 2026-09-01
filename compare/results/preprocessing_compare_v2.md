
════════════════════════════════════════════════════════════════════════
  TỔNG QUAN SO SÁNH: CODE vs PAPER
════════════════════════════════════════════════════════════════════════
  Instances paper          : 449
  Instances code           : 457
  Matched                  : 449
  Extra (chỉ trong code)  : 8
  Khớp hoàn toàn          : 37 / 449  (8%)

  Metric            Giống   Khác    Code✅   Paper⚠️  Hướng
  ──────────────── ────── ────── ──────── ─────────  ────────────────────
  ✅ Trivial LB        449      0        -         -  ↑ lớn hơn=tốt
  ✅ Trivial UB        449      0        -         -  ↓ nhỏ hơn=tốt
  ✅ Thorn Arcs        449      0        -         -  = bằng=đúng
  ❌ Cyclic LB         432     17       17         -  ↑ lớn hơn=tốt
  ❌ Arith UB           44    405      382        23  ↓ nhỏ hơn=tốt
  ❌ LNS UB            371     78       31        47  ↓ nhỏ hơn=tốt

────────────────────────────────────────────────────────────────────────
  KẾT LUẬN TỔNG THỂ
────────────────────────────────────────────────────────────────────────
  Trivial LB      : ✅ Khớp hoàn toàn
  Trivial UB      : ✅ Khớp hoàn toàn
  Thorn Arcs      : ✅ Khớp hoàn toàn
  Cyclic LB       : ✅ CODE tốt hơn  (17 vs 0)
  Arith UB        : ✅ CODE tốt hơn  (382 vs 23)
  LNS UB          : ⚠️  PAPER tốt hơn (47 vs 31)

────────────────────────────────────────────────────────────────────────
  THEO NHÓM INSTANCE
────────────────────────────────────────────────────────────────────────

  ┌─ [DRAND]  127 instances  |  17 khớp (13%)
  │  Cyclic LB     :  14 khác  [CODE 14 vs PAPER 0 → CODE✅]
  │  Arith UB      : 105 khác  [CODE 97 vs PAPER 8 → CODE✅]
  │  LNS UB        :  26 khác  [CODE 10 vs PAPER 16 → PAPER⚠️]
  └────────────────────────────────────────────────────────────

  ┌─ [FULL]  120 instances  |  9 khớp (7%)
  │  Arith UB      : 111 khác  [CODE 107 vs PAPER 4 → CODE✅]
  │  LNS UB        :  13 khác  [CODE 9 vs PAPER 4 → CODE✅]
  └────────────────────────────────────────────────────────────

  ┌─ [JOINT]  72 instances  |  2 khớp (2%)
  │  Cyclic LB     :   3 khác  [CODE 3 vs PAPER 0 → CODE✅]
  │  Arith UB      :  69 khác  [CODE 62 vs PAPER 7 → CODE✅]
  │  LNS UB        :  16 khác  [CODE 5 vs PAPER 11 → PAPER⚠️]
  └────────────────────────────────────────────────────────────

  ┌─ [MISS]  130 instances  |  9 khớp (6%)
  │  Arith UB      : 120 khác  [CODE 116 vs PAPER 4 → CODE✅]
  │  LNS UB        :  23 khác  [CODE 7 vs PAPER 16 → PAPER⚠️]
  └────────────────────────────────────────────────────────────

════════════════════════════════════════════════════════════════════════
  📁 FILE OUTPUT
════════════════════════════════════════════════════════════════════════
  📝 MD   → compare/results/preprocessing_compare.md

