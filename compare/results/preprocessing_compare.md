# Báo cáo So sánh: Code vs Paper
- **Thời gian**: 2026-09-01 15:01:51
- **File code**: `compare/results/prep_new_lns.md`
- **File paper**: `results/rp_preprocessing.csv`
---
## 1. Tổng quan
| Mục | Giá trị |
|---|---|
| Instances trong paper | 449 |
| Instances trong code | 457 |
| Matched (so sánh được) | 449 |
| Chỉ có trong code | 8 |
| Khớp hoàn toàn | **36 / 449** (8%) |

## 2. Tổng quan theo Metric
| Metric | Giống | Khác | Code tốt hơn | Paper tốt hơn | Hướng |
|---|---|---|---|---|---|
| ✅ Trivial LB | 449 | 0 | - | - | ↑ lớn hơn = tốt |
| ✅ Trivial UB | 449 | 0 | - | - | ↓ nhỏ hơn = tốt |
| ✅ Thorn Arcs | 449 | 0 | - | - | = bằng nhau |
| ❌ Cyclic LB | 432 | 17 | 17 | - | ↑ lớn hơn = tốt |
| ❌ Arith UB | 44 | 405 | 382 | 23 | ↓ nhỏ hơn = tốt |
| ❌ LNS UB | 388 | 61 | 32 | 29 | ↓ nhỏ hơn = tốt |

## 3. Kết luận: Ai tốt hơn tổng thể?
| Metric | Kết quả | Code tốt | Paper tốt |
|---|---|---|---|
| Trivial LB | ✅ Giống nhau hoàn toàn | - | - |
| Trivial UB | ✅ Giống nhau hoàn toàn | - | - |
| Thorn Arcs | ✅ Giống nhau hoàn toàn | - | - |
| Cyclic LB | ✅ **CODE tốt hơn** | 17 | 0 |
| Arith UB | ✅ **CODE tốt hơn** | 382 | 23 |
| LNS UB | ✅ **CODE tốt hơn** | 32 | 29 |

## 4. Thống kê theo Nhóm Instance
### Nhóm `DRAND` — 127 instances, 17 khớp hoàn toàn (13%)
| Metric | Khác | Code tốt | Paper tốt |
|---|---|---|---|
| Cyclic LB | 14 | 14 | 0 | ✅ CODE |
| Arith UB | 105 | 97 | 8 | ✅ CODE |
| LNS UB | 21 | 9 | 12 | ⚠️ PAPER |

### Nhóm `FULL` — 120 instances, 8 khớp hoàn toàn (6%)
| Metric | Khác | Code tốt | Paper tốt |
|---|---|---|---|
| Arith UB | 111 | 107 | 4 | ✅ CODE |
| LNS UB | 14 | 7 | 7 | 🟰 Ngang |

### Nhóm `JOINT` — 72 instances, 2 khớp hoàn toàn (2%)
| Metric | Khác | Code tốt | Paper tốt |
|---|---|---|---|
| Cyclic LB | 3 | 3 | 0 | ✅ CODE |
| Arith UB | 69 | 62 | 7 | ✅ CODE |
| LNS UB | 11 | 6 | 5 | ✅ CODE |

### Nhóm `MISS` — 130 instances, 9 khớp hoàn toàn (6%)
| Metric | Khác | Code tốt | Paper tốt |
|---|---|---|---|
| Arith UB | 120 | 116 | 4 | ✅ CODE |
| LNS UB | 15 | 10 | 5 | ✅ CODE |

## 5. Chi tiết từng Metric (instances khác nhau)
### Cyclic LB — 17 instances khác  (Code tốt: 17 | Paper tốt: 0)
| Instance | Nhóm | Paper | Code | Δ | Ai tốt |
|---|---|---|---|---|---|
| `drand-10-75-2` | drand | 5 | 6 | +1 | ✅ CODE |
| `drand-15-110-2` | drand | 6 | 7 | +1 | ✅ CODE |
| `drand-15-200-1` | drand | 6 | 7 | +1 | ✅ CODE |
| `drand-15-75-1` | drand | 6 | 7 | +1 | ✅ CODE |
| `drand-20-75-3` | drand | 7 | 8 | +1 | ✅ CODE |
| `drand-25-200-3` | drand | 8 | 9 | +1 | ✅ CODE |
| `drand-35-75-1` | drand | 9 | 10 | +1 | ✅ CODE |
| `drand-45-110-3` | drand | 10 | 11 | +1 | ✅ CODE |
| `drand-45-200-2` | drand | 10 | 11 | +1 | ✅ CODE |
| `drand-5-75-2` | drand | 4 | 5 | +1 | ✅ CODE |
| `drand-7-110-1` | drand | 5 | 6 | +1 | ✅ CODE |
| `drand-7-200-3` | drand | 5 | 6 | +1 | ✅ CODE |
| `drand-75-110-3` | drand | 13 | 14 | +1 | ✅ CODE |
| `drand-75-75-1` | drand | 13 | 14 | +1 | ✅ CODE |
| `joint-5-15-5-15-3` | joint | 7 | 8 | +1 | ✅ CODE |
| `joint-5-30-5-30-1` | joint | 7 | 8 | +1 | ✅ CODE |
| `joint-5-30-5-30-2` | joint | 7 | 8 | +1 | ✅ CODE |

### Arith UB — 405 instances khác  (Code tốt: 382 | Paper tốt: 23)
| Instance | Nhóm | Paper | Code | Δ | Ai tốt |
|---|---|---|---|---|---|
| `drand-10-110-2` | drand | 11 | 10 | -1 | ✅ CODE |
| `drand-10-75-1` | drand | 11 | 9 | -2 | ✅ CODE |
| `drand-10-75-2` | drand | 11 | 8 | -3 | ✅ CODE |
| `drand-10-75-3` | drand | 11 | 8 | -3 | ✅ CODE |
| `drand-100-110-1` | drand | 49 | 44 | -5 | ✅ CODE |
| `drand-100-200-1` | drand | 51 | 43 | -8 | ✅ CODE |
| `drand-100-200-2` | drand | 51 | 52 | +1 | ⚠️ PAPER |
| `drand-100-200-3` | drand | 58 | 49 | -9 | ✅ CODE |
| `drand-100-75-1` | drand | 41 | 40 | -1 | ✅ CODE |
| `drand-100-75-2` | drand | 39 | 40 | +1 | ⚠️ PAPER |
| `drand-112-110-1` | drand | 45 | 38 | -7 | ✅ CODE |
| `drand-112-110-2` | drand | 55 | 52 | -3 | ✅ CODE |
| `drand-112-110-3` | drand | 53 | 46 | -7 | ✅ CODE |
| `drand-112-200-1` | drand | 54 | 46 | -8 | ✅ CODE |
| `drand-112-200-2` | drand | 50 | 53 | +3 | ⚠️ PAPER |
| `drand-112-200-3` | drand | 58 | 51 | -7 | ✅ CODE |
| `drand-112-75-1` | drand | 57 | 52 | -5 | ✅ CODE |
| `drand-112-75-2` | drand | 42 | 51 | +9 | ⚠️ PAPER |
| `drand-112-75-3` | drand | 41 | 44 | +3 | ⚠️ PAPER |
| `drand-12-110-1` | drand | 13 | 9 | -4 | ✅ CODE |
| `drand-12-110-2` | drand | 13 | 11 | -2 | ✅ CODE |
| `drand-12-110-3` | drand | 13 | 11 | -2 | ✅ CODE |
| `drand-12-200-2` | drand | 13 | 11 | -2 | ✅ CODE |
| `drand-12-75-1` | drand | 11 | 9 | -2 | ✅ CODE |
| `drand-12-75-2` | drand | 13 | 10 | -3 | ✅ CODE |
| `drand-15-110-1` | drand | 16 | 13 | -3 | ✅ CODE |
| `drand-15-110-2` | drand | 16 | 13 | -3 | ✅ CODE |
| `drand-15-110-3` | drand | 16 | 15 | -1 | ✅ CODE |
| `drand-15-200-1` | drand | 15 | 16 | +1 | ⚠️ PAPER |
| `drand-15-200-2` | drand | 16 | 15 | -1 | ✅ CODE |
| `drand-15-200-3` | drand | 16 | 13 | -3 | ✅ CODE |
| `drand-15-75-1` | drand | 14 | 12 | -2 | ✅ CODE |
| `drand-15-75-2` | drand | 16 | 13 | -3 | ✅ CODE |
| `drand-15-75-3` | drand | 13 | 9 | -4 | ✅ CODE |
| `drand-20-110-1` | drand | 19 | 14 | -5 | ✅ CODE |
| `drand-20-110-2` | drand | 18 | 16 | -2 | ✅ CODE |
| `drand-20-110-3` | drand | 16 | 13 | -3 | ✅ CODE |
| `drand-20-200-1` | drand | 20 | 16 | -4 | ✅ CODE |
| `drand-20-200-2` | drand | 15 | 16 | +1 | ⚠️ PAPER |
| `drand-20-200-3` | drand | 21 | 15 | -6 | ✅ CODE |
| `drand-20-75-1` | drand | 20 | 15 | -5 | ✅ CODE |
| `drand-20-75-2` | drand | 17 | 13 | -4 | ✅ CODE |
| `drand-20-75-3` | drand | 18 | 14 | -4 | ✅ CODE |
| `drand-25-110-1` | drand | 20 | 12 | -8 | ✅ CODE |
| `drand-25-110-2` | drand | 20 | 15 | -5 | ✅ CODE |
| `drand-25-110-3` | drand | 20 | 14 | -6 | ✅ CODE |
| `drand-25-200-1` | drand | 23 | 17 | -6 | ✅ CODE |
| `drand-25-200-2` | drand | 25 | 21 | -4 | ✅ CODE |
| `drand-25-200-3` | drand | 24 | 16 | -8 | ✅ CODE |
| `drand-25-75-2` | drand | 18 | 15 | -3 | ✅ CODE |
| `drand-25-75-3` | drand | 22 | 17 | -5 | ✅ CODE |
| `drand-30-110-1` | drand | 25 | 16 | -9 | ✅ CODE |
| `drand-30-110-2` | drand | 24 | 19 | -5 | ✅ CODE |
| `drand-30-110-3` | drand | 21 | 17 | -4 | ✅ CODE |
| `drand-30-200-1` | drand | 28 | 22 | -6 | ✅ CODE |
| `drand-30-200-2` | drand | 28 | 20 | -8 | ✅ CODE |
| `drand-30-200-3` | drand | 25 | 19 | -6 | ✅ CODE |
| `drand-30-75-1` | drand | 22 | 14 | -8 | ✅ CODE |
| `drand-30-75-2` | drand | 22 | 21 | -1 | ✅ CODE |
| `drand-30-75-3` | drand | 19 | 18 | -1 | ✅ CODE |
| `drand-35-110-1` | drand | 23 | 18 | -5 | ✅ CODE |
| `drand-35-110-2` | drand | 25 | 24 | -1 | ✅ CODE |
| `drand-35-110-3` | drand | 24 | 21 | -3 | ✅ CODE |
| `drand-35-200-1` | drand | 32 | 25 | -7 | ✅ CODE |
| `drand-35-200-2` | drand | 30 | 21 | -9 | ✅ CODE |
| `drand-35-200-3` | drand | 28 | 23 | -5 | ✅ CODE |
| `drand-35-75-1` | drand | 24 | 20 | -4 | ✅ CODE |
| `drand-35-75-2` | drand | 23 | 19 | -4 | ✅ CODE |
| `drand-35-75-3` | drand | 21 | 17 | -4 | ✅ CODE |
| `drand-40-110-1` | drand | 24 | 20 | -4 | ✅ CODE |
| `drand-40-110-2` | drand | 23 | 20 | -3 | ✅ CODE |
| `drand-40-110-3` | drand | 25 | 20 | -5 | ✅ CODE |
| `drand-40-200-1` | drand | 30 | 24 | -6 | ✅ CODE |
| `drand-40-200-2` | drand | 27 | 24 | -3 | ✅ CODE |
| `drand-40-200-3` | drand | 31 | 23 | -8 | ✅ CODE |
| `drand-40-75-1` | drand | 24 | 19 | -5 | ✅ CODE |
| `drand-40-75-2` | drand | 26 | 20 | -6 | ✅ CODE |
| `drand-40-75-3` | drand | 24 | 19 | -5 | ✅ CODE |
| `drand-45-110-1` | drand | 31 | 22 | -9 | ✅ CODE |
| `drand-45-110-2` | drand | 26 | 23 | -3 | ✅ CODE |
| `drand-45-110-3` | drand | 27 | 22 | -5 | ✅ CODE |
| `drand-45-200-1` | drand | 33 | 26 | -7 | ✅ CODE |
| `drand-45-200-2` | drand | 33 | 27 | -6 | ✅ CODE |
| `drand-45-200-3` | drand | 34 | 26 | -8 | ✅ CODE |
| `drand-45-75-1` | drand | 27 | 20 | -7 | ✅ CODE |
| `drand-45-75-2` | drand | 25 | 20 | -5 | ✅ CODE |
| `drand-45-75-3` | drand | 24 | 22 | -2 | ✅ CODE |
| `drand-50-110-1` | drand | 35 | 28 | -7 | ✅ CODE |
| `drand-50-110-2` | drand | 31 | 25 | -6 | ✅ CODE |
| `drand-50-110-3` | drand | 29 | 25 | -4 | ✅ CODE |
| `drand-50-200-1` | drand | 33 | 31 | -2 | ✅ CODE |
| `drand-50-200-2` | drand | 37 | 29 | -8 | ✅ CODE |
| `drand-50-200-3` | drand | 40 | 33 | -7 | ✅ CODE |
| `drand-50-75-1` | drand | 29 | 22 | -7 | ✅ CODE |
| `drand-50-75-2` | drand | 27 | 24 | -3 | ✅ CODE |
| `drand-50-75-3` | drand | 32 | 24 | -8 | ✅ CODE |
| `drand-75-110-1` | drand | 35 | 31 | -4 | ✅ CODE |
| `drand-75-110-2` | drand | 43 | 34 | -9 | ✅ CODE |
| `drand-75-110-3` | drand | 40 | 35 | -5 | ✅ CODE |
| `drand-75-200-1` | drand | 40 | 31 | -9 | ✅ CODE |
| `drand-75-200-2` | drand | 45 | 41 | -4 | ✅ CODE |
| `drand-75-200-3` | drand | 45 | 35 | -10 | ✅ CODE |
| `drand-75-75-1` | drand | 33 | 29 | -4 | ✅ CODE |
| `drand-75-75-2` | drand | 30 | 36 | +6 | ⚠️ PAPER |
| `drand-75-75-3` | drand | 39 | 33 | -6 | ✅ CODE |
| `full-10-15-1` | full | 27 | 19 | -8 | ✅ CODE |
| `full-10-15-2` | full | 29 | 20 | -9 | ✅ CODE |
| `full-10-15-3` | full | 24 | 19 | -5 | ✅ CODE |
| `full-10-15-4` | full | 19 | 20 | +1 | ⚠️ PAPER |
| `full-10-15-5` | full | 25 | 21 | -4 | ✅ CODE |
| `full-10-30-1` | full | 27 | 16 | -11 | ✅ CODE |
| `full-10-30-2` | full | 27 | 15 | -12 | ✅ CODE |
| `full-10-30-3` | full | 25 | 19 | -6 | ✅ CODE |
| `full-10-30-4` | full | 26 | 14 | -12 | ✅ CODE |
| `full-10-30-5` | full | 31 | 20 | -11 | ✅ CODE |
| `full-11-15-1` | full | 27 | 21 | -6 | ✅ CODE |
| `full-11-15-2` | full | 27 | 20 | -7 | ✅ CODE |
| `full-11-15-3` | full | 29 | 19 | -10 | ✅ CODE |
| `full-11-15-4` | full | 32 | 23 | -9 | ✅ CODE |
| `full-11-15-5` | full | 28 | 22 | -6 | ✅ CODE |
| `full-11-30-1` | full | 34 | 25 | -9 | ✅ CODE |
| `full-11-30-2` | full | 30 | 24 | -6 | ✅ CODE |
| `full-11-30-3` | full | 32 | 19 | -13 | ✅ CODE |
| `full-11-30-4` | full | 37 | 23 | -14 | ✅ CODE |
| `full-11-30-5` | full | 33 | 22 | -11 | ✅ CODE |
| `full-12-15-1` | full | 32 | 25 | -7 | ✅ CODE |
| `full-12-15-2` | full | 24 | 27 | +3 | ⚠️ PAPER |
| `full-12-15-3` | full | 27 | 23 | -4 | ✅ CODE |
| `full-12-15-4` | full | 30 | 26 | -4 | ✅ CODE |
| `full-12-15-5` | full | 36 | 29 | -7 | ✅ CODE |
| `full-12-30-1` | full | 33 | 26 | -7 | ✅ CODE |
| `full-12-30-2` | full | 31 | 26 | -5 | ✅ CODE |
| `full-12-30-3` | full | 38 | 29 | -9 | ✅ CODE |
| `full-12-30-4` | full | 39 | 25 | -14 | ✅ CODE |
| `full-12-30-5` | full | 41 | 26 | -15 | ✅ CODE |
| `full-13-15-1` | full | 41 | 30 | -11 | ✅ CODE |
| `full-13-15-2` | full | 40 | 31 | -9 | ✅ CODE |
| `full-13-15-3` | full | 35 | 30 | -5 | ✅ CODE |
| `full-13-15-4` | full | 34 | 31 | -3 | ✅ CODE |
| `full-13-15-5` | full | 39 | 36 | -3 | ✅ CODE |
| `full-13-30-1` | full | 39 | 28 | -11 | ✅ CODE |
| `full-13-30-2` | full | 36 | 29 | -7 | ✅ CODE |
| `full-13-30-3` | full | 36 | 26 | -10 | ✅ CODE |
| `full-13-30-4` | full | 36 | 29 | -7 | ✅ CODE |
| `full-13-30-5` | full | 36 | 31 | -5 | ✅ CODE |
| `full-14-15-1` | full | 34 | 33 | -1 | ✅ CODE |
| `full-14-15-2` | full | 38 | 34 | -4 | ✅ CODE |
| `full-14-15-3` | full | 32 | 33 | +1 | ⚠️ PAPER |
| `full-14-15-4` | full | 32 | 28 | -4 | ✅ CODE |
| `full-14-15-5` | full | 38 | 35 | -3 | ✅ CODE |
| `full-14-30-1` | full | 38 | 28 | -10 | ✅ CODE |
| `full-14-30-2` | full | 42 | 31 | -11 | ✅ CODE |
| `full-14-30-3` | full | 38 | 30 | -8 | ✅ CODE |
| `full-14-30-4` | full | 35 | 30 | -5 | ✅ CODE |
| `full-14-30-5` | full | 37 | 30 | -7 | ✅ CODE |
| `full-15-15-2` | full | 54 | 52 | -2 | ✅ CODE |
| `full-15-15-3` | full | 47 | 43 | -4 | ✅ CODE |
| `full-15-15-4` | full | 37 | 39 | +2 | ⚠️ PAPER |
| `full-15-15-5` | full | 39 | 29 | -10 | ✅ CODE |
| `full-15-30-1` | full | 50 | 37 | -13 | ✅ CODE |
| `full-15-30-2` | full | 53 | 46 | -7 | ✅ CODE |
| `full-15-30-3` | full | 52 | 39 | -13 | ✅ CODE |
| `full-15-30-4` | full | 50 | 38 | -12 | ✅ CODE |
| `full-15-30-5` | full | 47 | 43 | -4 | ✅ CODE |
| `full-16-15-1` | full | 56 | 50 | -6 | ✅ CODE |
| `full-16-15-2` | full | 55 | 44 | -11 | ✅ CODE |
| `full-16-15-3` | full | 49 | 47 | -2 | ✅ CODE |
| `full-16-15-4` | full | 50 | 40 | -10 | ✅ CODE |
| `full-16-15-5` | full | 53 | 42 | -11 | ✅ CODE |
| `full-16-30-1` | full | 58 | 44 | -14 | ✅ CODE |
| `full-16-30-2` | full | 44 | 38 | -6 | ✅ CODE |
| `full-16-30-3` | full | 49 | 37 | -12 | ✅ CODE |
| `full-16-30-4` | full | 44 | 37 | -7 | ✅ CODE |
| `full-16-30-5` | full | 52 | 44 | -8 | ✅ CODE |
| `full-5-15-3` | full | 9 | 5 | -4 | ✅ CODE |
| `full-5-15-4` | full | 11 | 8 | -3 | ✅ CODE |
| `full-5-30-1` | full | 6 | 5 | -1 | ✅ CODE |
| `full-5-30-2` | full | 10 | 7 | -3 | ✅ CODE |
| `full-5-30-3` | full | 6 | 5 | -1 | ✅ CODE |
| `full-5-30-5` | full | 9 | 7 | -2 | ✅ CODE |
| `full-6-15-1` | full | 12 | 6 | -6 | ✅ CODE |
| `full-6-15-2` | full | 16 | 13 | -3 | ✅ CODE |
| `full-6-15-3` | full | 10 | 6 | -4 | ✅ CODE |
| `full-6-15-4` | full | 11 | 10 | -1 | ✅ CODE |
| `full-6-30-1` | full | 16 | 13 | -3 | ✅ CODE |
| `full-6-30-2` | full | 16 | 6 | -10 | ✅ CODE |
| `full-6-30-4` | full | 13 | 9 | -4 | ✅ CODE |
| `full-6-30-5` | full | 13 | 6 | -7 | ✅ CODE |
| `full-7-15-1` | full | 16 | 9 | -7 | ✅ CODE |
| `full-7-15-2` | full | 12 | 10 | -2 | ✅ CODE |
| `full-7-15-3` | full | 14 | 10 | -4 | ✅ CODE |
| `full-7-15-4` | full | 15 | 12 | -3 | ✅ CODE |
| `full-7-15-5` | full | 12 | 11 | -1 | ✅ CODE |
| `full-7-30-1` | full | 19 | 11 | -8 | ✅ CODE |
| `full-7-30-2` | full | 16 | 11 | -5 | ✅ CODE |
| `full-7-30-3` | full | 16 | 10 | -6 | ✅ CODE |
| `full-7-30-4` | full | 17 | 9 | -8 | ✅ CODE |
| `full-7-30-5` | full | 13 | 7 | -6 | ✅ CODE |
| `full-8-15-1` | full | 22 | 17 | -5 | ✅ CODE |
| `full-8-15-2` | full | 14 | 12 | -2 | ✅ CODE |
| `full-8-15-3` | full | 18 | 13 | -5 | ✅ CODE |
| `full-8-15-4` | full | 16 | 11 | -5 | ✅ CODE |
| `full-8-15-5` | full | 16 | 8 | -8 | ✅ CODE |
| `full-8-30-2` | full | 22 | 13 | -9 | ✅ CODE |
| `full-8-30-3` | full | 17 | 12 | -5 | ✅ CODE |
| `full-8-30-4` | full | 20 | 12 | -8 | ✅ CODE |
| `full-8-30-5` | full | 20 | 13 | -7 | ✅ CODE |
| `full-9-15-2` | full | 19 | 14 | -5 | ✅ CODE |
| `full-9-15-3` | full | 26 | 18 | -8 | ✅ CODE |
| `full-9-15-4` | full | 20 | 16 | -4 | ✅ CODE |
| `full-9-15-5` | full | 17 | 14 | -3 | ✅ CODE |
| `full-9-30-1` | full | 25 | 17 | -8 | ✅ CODE |
| `full-9-30-2` | full | 18 | 14 | -4 | ✅ CODE |
| `full-9-30-3` | full | 23 | 19 | -4 | ✅ CODE |
| `full-9-30-4` | full | 17 | 16 | -1 | ✅ CODE |
| `full-9-30-5` | full | 26 | 17 | -9 | ✅ CODE |
| `joint-10-15-5-15-1` | joint | 32 | 27 | -5 | ✅ CODE |
| `joint-10-15-5-15-3` | joint | 30 | 26 | -4 | ✅ CODE |
| `joint-10-30-5-30-1` | joint | 29 | 24 | -5 | ✅ CODE |
| `joint-10-30-5-30-2` | joint | 39 | 22 | -17 | ✅ CODE |
| `joint-10-30-5-30-3` | joint | 28 | 25 | -3 | ✅ CODE |
| `joint-11-15-5-15-1` | joint | 33 | 25 | -8 | ✅ CODE |
| `joint-11-15-5-15-2` | joint | 30 | 31 | +1 | ⚠️ PAPER |
| `joint-11-15-5-15-3` | joint | 33 | 29 | -4 | ✅ CODE |
| `joint-11-30-5-30-1` | joint | 37 | 23 | -14 | ✅ CODE |
| `joint-11-30-5-30-2` | joint | 33 | 27 | -6 | ✅ CODE |
| `joint-11-30-5-30-3` | joint | 28 | 27 | -1 | ✅ CODE |
| `joint-12-15-5-15-1` | joint | 36 | 28 | -8 | ✅ CODE |
| `joint-12-15-5-15-2` | joint | 30 | 31 | +1 | ⚠️ PAPER |
| `joint-12-15-5-15-3` | joint | 33 | 30 | -3 | ✅ CODE |
| `joint-12-30-5-30-1` | joint | 36 | 30 | -6 | ✅ CODE |
| `joint-12-30-5-30-2` | joint | 34 | 29 | -5 | ✅ CODE |
| `joint-12-30-5-30-3` | joint | 35 | 29 | -6 | ✅ CODE |
| `joint-13-15-5-15-1` | joint | 42 | 30 | -12 | ✅ CODE |
| `joint-13-15-5-15-2` | joint | 44 | 35 | -9 | ✅ CODE |
| `joint-13-15-5-15-3` | joint | 43 | 39 | -4 | ✅ CODE |
| `joint-13-30-5-30-1` | joint | 35 | 37 | +2 | ⚠️ PAPER |
| `joint-13-30-5-30-2` | joint | 41 | 36 | -5 | ✅ CODE |
| `joint-13-30-5-30-3` | joint | 47 | 37 | -10 | ✅ CODE |
| `joint-14-15-5-15-1` | joint | 41 | 37 | -4 | ✅ CODE |
| `joint-14-15-5-15-2` | joint | 37 | 45 | +8 | ⚠️ PAPER |
| `joint-14-15-5-15-3` | joint | 43 | 33 | -10 | ✅ CODE |
| `joint-14-30-5-30-1` | joint | 47 | 39 | -8 | ✅ CODE |
| `joint-14-30-5-30-2` | joint | 46 | 40 | -6 | ✅ CODE |
| `joint-14-30-5-30-3` | joint | 54 | 46 | -8 | ✅ CODE |
| `joint-15-15-5-15-1` | joint | 41 | 34 | -7 | ✅ CODE |
| `joint-15-15-5-15-2` | joint | 46 | 47 | +1 | ⚠️ PAPER |
| `joint-15-15-5-15-3` | joint | 57 | 46 | -11 | ✅ CODE |
| `joint-15-30-5-30-1` | joint | 53 | 38 | -15 | ✅ CODE |
| `joint-15-30-5-30-3` | joint | 48 | 42 | -6 | ✅ CODE |
| `joint-16-15-5-15-1` | joint | 60 | 56 | -4 | ✅ CODE |
| `joint-16-15-5-15-2` | joint | 59 | 54 | -5 | ✅ CODE |
| `joint-16-15-5-15-3` | joint | 56 | 40 | -16 | ✅ CODE |
| `joint-16-30-5-30-1` | joint | 44 | 46 | +2 | ⚠️ PAPER |
| `joint-16-30-5-30-2` | joint | 48 | 41 | -7 | ✅ CODE |
| `joint-16-30-5-30-3` | joint | 64 | 45 | -19 | ✅ CODE |
| `joint-5-15-5-15-2` | joint | 16 | 15 | -1 | ✅ CODE |
| `joint-5-15-5-15-3` | joint | 19 | 15 | -4 | ✅ CODE |
| `joint-5-30-5-30-1` | joint | 17 | 14 | -3 | ✅ CODE |
| `joint-5-30-5-30-2` | joint | 16 | 17 | +1 | ⚠️ PAPER |
| `joint-5-30-5-30-3` | joint | 17 | 15 | -2 | ✅ CODE |
| `joint-6-15-5-15-1` | joint | 22 | 16 | -6 | ✅ CODE |
| `joint-6-15-5-15-2` | joint | 20 | 13 | -7 | ✅ CODE |
| `joint-6-15-5-15-3` | joint | 24 | 19 | -5 | ✅ CODE |
| `joint-6-30-5-30-1` | joint | 24 | 16 | -8 | ✅ CODE |
| `joint-6-30-5-30-2` | joint | 14 | 11 | -3 | ✅ CODE |
| `joint-6-30-5-30-3` | joint | 20 | 15 | -5 | ✅ CODE |
| `joint-7-15-5-15-1` | joint | 24 | 16 | -8 | ✅ CODE |
| `joint-7-15-5-15-2` | joint | 24 | 16 | -8 | ✅ CODE |
| `joint-7-15-5-15-3` | joint | 22 | 14 | -8 | ✅ CODE |
| `joint-7-30-5-30-1` | joint | 19 | 13 | -6 | ✅ CODE |
| `joint-7-30-5-30-2` | joint | 18 | 15 | -3 | ✅ CODE |
| `joint-7-30-5-30-3` | joint | 22 | 15 | -7 | ✅ CODE |
| `joint-8-15-5-15-1` | joint | 22 | 17 | -5 | ✅ CODE |
| `joint-8-15-5-15-2` | joint | 22 | 18 | -4 | ✅ CODE |
| `joint-8-15-5-15-3` | joint | 21 | 19 | -2 | ✅ CODE |
| `joint-8-30-5-30-1` | joint | 22 | 20 | -2 | ✅ CODE |
| `joint-8-30-5-30-2` | joint | 22 | 15 | -7 | ✅ CODE |
| `joint-8-30-5-30-3` | joint | 23 | 17 | -6 | ✅ CODE |
| `joint-9-15-5-15-1` | joint | 27 | 16 | -11 | ✅ CODE |
| `joint-9-15-5-15-2` | joint | 27 | 22 | -5 | ✅ CODE |
| `joint-9-15-5-15-3` | joint | 25 | 19 | -6 | ✅ CODE |
| `joint-9-30-5-30-1` | joint | 27 | 24 | -3 | ✅ CODE |
| `joint-9-30-5-30-2` | joint | 32 | 23 | -9 | ✅ CODE |
| `joint-9-30-5-30-3` | joint | 27 | 21 | -6 | ✅ CODE |
| `miss-10-15-4-1` | miss | 20 | 16 | -4 | ✅ CODE |
| `miss-10-15-4-2` | miss | 23 | 16 | -7 | ✅ CODE |
| `miss-10-15-4-3` | miss | 26 | 17 | -9 | ✅ CODE |
| `miss-10-15-4-4` | miss | 27 | 22 | -5 | ✅ CODE |
| `miss-10-15-4-5` | miss | 24 | 21 | -3 | ✅ CODE |
| `miss-10-30-4-1` | miss | 26 | 18 | -8 | ✅ CODE |
| `miss-10-30-4-2` | miss | 29 | 17 | -12 | ✅ CODE |
| `miss-10-30-4-3` | miss | 30 | 19 | -11 | ✅ CODE |
| `miss-10-30-4-4` | miss | 26 | 21 | -5 | ✅ CODE |
| `miss-10-30-4-5` | miss | 24 | 18 | -6 | ✅ CODE |
| `miss-11-15-5-1` | miss | 28 | 17 | -11 | ✅ CODE |
| `miss-11-15-5-2` | miss | 24 | 19 | -5 | ✅ CODE |
| `miss-11-15-5-3` | miss | 27 | 21 | -6 | ✅ CODE |
| `miss-11-15-5-4` | miss | 23 | 17 | -6 | ✅ CODE |
| `miss-11-15-5-5` | miss | 33 | 24 | -9 | ✅ CODE |
| `miss-11-30-5-1` | miss | 29 | 22 | -7 | ✅ CODE |
| `miss-11-30-5-2` | miss | 35 | 26 | -9 | ✅ CODE |
| `miss-11-30-5-3` | miss | 29 | 23 | -6 | ✅ CODE |
| `miss-11-30-5-4` | miss | 32 | 19 | -13 | ✅ CODE |
| `miss-11-30-5-5` | miss | 26 | 18 | -8 | ✅ CODE |
| `miss-12-15-5-1` | miss | 30 | 23 | -7 | ✅ CODE |
| `miss-12-15-5-2` | miss | 30 | 24 | -6 | ✅ CODE |
| `miss-12-15-5-3` | miss | 29 | 23 | -6 | ✅ CODE |
| `miss-12-15-5-4` | miss | 27 | 21 | -6 | ✅ CODE |
| `miss-12-15-5-5` | miss | 31 | 21 | -10 | ✅ CODE |
| `miss-12-30-5-1` | miss | 36 | 24 | -12 | ✅ CODE |
| `miss-12-30-5-2` | miss | 31 | 24 | -7 | ✅ CODE |
| `miss-12-30-5-3` | miss | 27 | 16 | -11 | ✅ CODE |
| `miss-12-30-5-4` | miss | 30 | 26 | -4 | ✅ CODE |
| `miss-12-30-5-5` | miss | 39 | 30 | -9 | ✅ CODE |
| `miss-13-15-6-1` | miss | 34 | 26 | -8 | ✅ CODE |
| `miss-13-15-6-2` | miss | 32 | 24 | -8 | ✅ CODE |
| `miss-13-15-6-3` | miss | 34 | 26 | -8 | ✅ CODE |
| `miss-13-15-6-4` | miss | 36 | 29 | -7 | ✅ CODE |
| `miss-13-15-6-5` | miss | 34 | 33 | -1 | ✅ CODE |
| `miss-13-30-6-1` | miss | 37 | 29 | -8 | ✅ CODE |
| `miss-13-30-6-2` | miss | 43 | 26 | -17 | ✅ CODE |
| `miss-13-30-6-3` | miss | 47 | 37 | -10 | ✅ CODE |
| `miss-13-30-6-4` | miss | 32 | 28 | -4 | ✅ CODE |
| `miss-13-30-6-5` | miss | 40 | 36 | -4 | ✅ CODE |
| `miss-14-15-6-1` | miss | 36 | 30 | -6 | ✅ CODE |
| `miss-14-15-6-2` | miss | 41 | 34 | -7 | ✅ CODE |
| `miss-14-15-6-3` | miss | 46 | 32 | -14 | ✅ CODE |
| `miss-14-15-6-4` | miss | 41 | 40 | -1 | ✅ CODE |
| `miss-14-15-6-5` | miss | 41 | 32 | -9 | ✅ CODE |
| `miss-14-30-6-1` | miss | 45 | 32 | -13 | ✅ CODE |
| `miss-14-30-6-2` | miss | 45 | 30 | -15 | ✅ CODE |
| `miss-14-30-6-3` | miss | 39 | 31 | -8 | ✅ CODE |
| `miss-14-30-6-4` | miss | 38 | 33 | -5 | ✅ CODE |
| `miss-14-30-6-5` | miss | 40 | 30 | -10 | ✅ CODE |
| `miss-15-15-7-1` | miss | 48 | 42 | -6 | ✅ CODE |
| `miss-15-15-7-2` | miss | 44 | 39 | -5 | ✅ CODE |
| `miss-15-15-7-3` | miss | 41 | 38 | -3 | ✅ CODE |
| `miss-15-15-7-4` | miss | 44 | 36 | -8 | ✅ CODE |
| `miss-15-15-7-5` | miss | 41 | 37 | -4 | ✅ CODE |
| `miss-15-30-7-1` | miss | 43 | 38 | -5 | ✅ CODE |
| `miss-15-30-7-2` | miss | 40 | 37 | -3 | ✅ CODE |
| `miss-15-30-7-4` | miss | 50 | 42 | -8 | ✅ CODE |
| `miss-16-15-7-1` | miss | 45 | 43 | -2 | ✅ CODE |
| `miss-16-15-7-2` | miss | 51 | 43 | -8 | ✅ CODE |
| `miss-16-15-7-3` | miss | 36 | 41 | +5 | ⚠️ PAPER |
| `miss-16-15-7-4` | miss | 52 | 48 | -4 | ✅ CODE |
| `miss-16-15-7-5` | miss | 38 | 44 | +6 | ⚠️ PAPER |
| `miss-16-30-7-1` | miss | 50 | 44 | -6 | ✅ CODE |
| `miss-16-30-7-2` | miss | 49 | 42 | -7 | ✅ CODE |
| `miss-16-30-7-3` | miss | 46 | 39 | -7 | ✅ CODE |
| `miss-16-30-7-4` | miss | 45 | 36 | -9 | ✅ CODE |
| `miss-16-30-7-5` | miss | 47 | 39 | -8 | ✅ CODE |
| `miss-17-15-8-1` | miss | 47 | 51 | +4 | ⚠️ PAPER |
| `miss-17-15-8-3` | miss | 48 | 47 | -1 | ✅ CODE |
| `miss-17-15-8-4` | miss | 58 | 55 | -3 | ✅ CODE |
| `miss-17-15-8-5` | miss | 54 | 50 | -4 | ✅ CODE |
| `miss-17-30-8-1` | miss | 53 | 50 | -3 | ✅ CODE |
| `miss-17-30-8-2` | miss | 63 | 50 | -13 | ✅ CODE |
| `miss-17-30-8-3` | miss | 55 | 45 | -10 | ✅ CODE |
| `miss-17-30-8-4` | miss | 50 | 46 | -4 | ✅ CODE |
| `miss-17-30-8-5` | miss | 50 | 45 | -5 | ✅ CODE |
| `miss-5-15-2-5` | miss | 6 | 5 | -1 | ✅ CODE |
| `miss-5-30-2-1` | miss | 9 | 6 | -3 | ✅ CODE |
| `miss-5-30-2-3` | miss | 9 | 7 | -2 | ✅ CODE |
| `miss-5-30-2-4` | miss | 9 | 7 | -2 | ✅ CODE |
| `miss-5-30-2-5` | miss | 9 | 8 | -1 | ✅ CODE |
| `miss-6-15-2-1` | miss | 11 | 8 | -3 | ✅ CODE |
| `miss-6-15-2-2` | miss | 8 | 6 | -2 | ✅ CODE |
| `miss-6-15-2-3` | miss | 9 | 6 | -3 | ✅ CODE |
| `miss-6-15-2-4` | miss | 13 | 9 | -4 | ✅ CODE |
| `miss-6-15-2-5` | miss | 8 | 9 | +1 | ⚠️ PAPER |
| `miss-6-30-2-1` | miss | 11 | 8 | -3 | ✅ CODE |
| `miss-6-30-2-2` | miss | 12 | 7 | -5 | ✅ CODE |
| `miss-6-30-2-3` | miss | 14 | 11 | -3 | ✅ CODE |
| `miss-6-30-2-4` | miss | 11 | 9 | -2 | ✅ CODE |
| `miss-6-30-2-5` | miss | 14 | 10 | -4 | ✅ CODE |
| `miss-7-15-3-1` | miss | 12 | 9 | -3 | ✅ CODE |
| `miss-7-15-3-3` | miss | 15 | 12 | -3 | ✅ CODE |
| `miss-7-15-3-5` | miss | 16 | 10 | -6 | ✅ CODE |
| `miss-7-30-3-1` | miss | 19 | 7 | -12 | ✅ CODE |
| `miss-7-30-3-2` | miss | 16 | 10 | -6 | ✅ CODE |
| `miss-7-30-3-3` | miss | 13 | 11 | -2 | ✅ CODE |
| `miss-7-30-3-4` | miss | 19 | 12 | -7 | ✅ CODE |
| `miss-7-30-3-5` | miss | 18 | 13 | -5 | ✅ CODE |
| `miss-8-15-3-1` | miss | 19 | 15 | -4 | ✅ CODE |
| `miss-8-15-3-2` | miss | 17 | 11 | -6 | ✅ CODE |
| `miss-8-15-3-3` | miss | 18 | 16 | -2 | ✅ CODE |
| `miss-8-15-3-4` | miss | 15 | 11 | -4 | ✅ CODE |
| `miss-8-15-3-5` | miss | 17 | 11 | -6 | ✅ CODE |
| `miss-8-30-3-1` | miss | 19 | 16 | -3 | ✅ CODE |
| `miss-8-30-3-2` | miss | 19 | 13 | -6 | ✅ CODE |
| `miss-8-30-3-3` | miss | 18 | 13 | -5 | ✅ CODE |
| `miss-8-30-3-4` | miss | 20 | 12 | -8 | ✅ CODE |
| `miss-8-30-3-5` | miss | 19 | 12 | -7 | ✅ CODE |
| `miss-9-15-4-1` | miss | 19 | 13 | -6 | ✅ CODE |
| `miss-9-15-4-2` | miss | 22 | 16 | -6 | ✅ CODE |
| `miss-9-15-4-3` | miss | 27 | 16 | -11 | ✅ CODE |
| `miss-9-15-4-4` | miss | 22 | 13 | -9 | ✅ CODE |
| `miss-9-15-4-5` | miss | 28 | 19 | -9 | ✅ CODE |
| `miss-9-30-4-1` | miss | 23 | 19 | -4 | ✅ CODE |
| `miss-9-30-4-2` | miss | 19 | 15 | -4 | ✅ CODE |
| `miss-9-30-4-3` | miss | 26 | 19 | -7 | ✅ CODE |
| `miss-9-30-4-4` | miss | 24 | 13 | -11 | ✅ CODE |
| `miss-9-30-4-5` | miss | 24 | 13 | -11 | ✅ CODE |

### LNS UB — 61 instances khác  (Code tốt: 32 | Paper tốt: 29)
| Instance | Nhóm | Paper | Code | Δ | Ai tốt |
|---|---|---|---|---|---|
| `drand-10-200-3` | drand | 8 | 9 | +1 | ⚠️ PAPER |
| `drand-100-110-1` | drand | 21 | 22 | +1 | ⚠️ PAPER |
| `drand-100-110-2` | drand | 22 | 21 | -1 | ✅ CODE |
| `drand-100-200-2` | drand | 25 | 24 | -1 | ✅ CODE |
| `drand-100-75-2` | drand | 20 | 19 | -1 | ✅ CODE |
| `drand-112-110-3` | drand | 22 | 23 | +1 | ⚠️ PAPER |
| `drand-112-200-1` | drand | 24 | 25 | +1 | ⚠️ PAPER |
| `drand-112-200-2` | drand | 24 | 25 | +1 | ⚠️ PAPER |
| `drand-112-200-3` | drand | 25 | 26 | +1 | ⚠️ PAPER |
| `drand-112-75-1` | drand | 21 | 22 | +1 | ⚠️ PAPER |
| `drand-112-75-3` | drand | 21 | 20 | -1 | ✅ CODE |
| `drand-12-110-1` | drand | 8 | 9 | +1 | ⚠️ PAPER |
| `drand-12-110-2` | drand | 8 | 9 | +1 | ⚠️ PAPER |
| `drand-20-75-2` | drand | 10 | 11 | +1 | ⚠️ PAPER |
| `drand-30-110-2` | drand | 13 | 12 | -1 | ✅ CODE |
| `drand-30-200-2` | drand | 14 | 13 | -1 | ✅ CODE |
| `drand-40-200-3` | drand | 16 | 15 | -1 | ✅ CODE |
| `drand-45-75-2` | drand | 15 | 14 | -1 | ✅ CODE |
| `drand-50-200-2` | drand | 17 | 18 | +1 | ⚠️ PAPER |
| `drand-50-75-2` | drand | 16 | 15 | -1 | ✅ CODE |
| `drand-50-75-3` | drand | 15 | 16 | +1 | ⚠️ PAPER |
| `full-11-15-1` | full | 12 | 11 | -1 | ✅ CODE |
| `full-13-15-1` | full | 13 | 14 | +1 | ⚠️ PAPER |
| `full-13-15-3` | full | 14 | 13 | -1 | ✅ CODE |
| `full-14-30-2` | full | 15 | 14 | -1 | ✅ CODE |
| `full-15-15-1` | full | 16 | 15 | -1 | ✅ CODE |
| `full-15-15-3` | full | 15 | 16 | +1 | ⚠️ PAPER |
| `full-15-15-4` | full | 15 | 16 | +1 | ⚠️ PAPER |
| `full-15-15-5` | full | 16 | 17 | +1 | ⚠️ PAPER |
| `full-16-15-1` | full | 19 | 16 | -3 | ✅ CODE |
| `full-16-15-2` | full | 17 | 18 | +1 | ⚠️ PAPER |
| `full-16-15-3` | full | 16 | 17 | +1 | ⚠️ PAPER |
| `full-16-15-4` | full | 16 | 19 | +3 | ⚠️ PAPER |
| `full-16-30-1` | full | 17 | 16 | -1 | ✅ CODE |
| `full-16-30-5` | full | 17 | 16 | -1 | ✅ CODE |
| `joint-11-15-5-15-2` | joint | 14 | 15 | +1 | ⚠️ PAPER |
| `joint-12-15-5-15-2` | joint | 15 | 16 | +1 | ⚠️ PAPER |
| `joint-12-15-5-15-3` | joint | 15 | 16 | +1 | ⚠️ PAPER |
| `joint-13-30-5-30-1` | joint | 17 | 16 | -1 | ✅ CODE |
| `joint-14-15-5-15-3` | joint | 18 | 17 | -1 | ✅ CODE |
| `joint-14-30-5-30-2` | joint | 18 | 17 | -1 | ✅ CODE |
| `joint-15-15-5-15-1` | joint | 20 | 19 | -1 | ✅ CODE |
| `joint-15-30-5-30-2` | joint | 19 | 18 | -1 | ✅ CODE |
| `joint-16-15-5-15-2` | joint | 20 | 19 | -1 | ✅ CODE |
| `joint-16-15-5-15-3` | joint | 19 | 20 | +1 | ⚠️ PAPER |
| `joint-7-30-5-30-1` | joint | 10 | 11 | +1 | ⚠️ PAPER |
| `miss-12-15-5-3` | miss | 13 | 12 | -1 | ✅ CODE |
| `miss-13-15-6-3` | miss | 13 | 14 | +1 | ⚠️ PAPER |
| `miss-13-15-6-5` | miss | 14 | 15 | +1 | ⚠️ PAPER |
| `miss-14-15-6-1` | miss | 15 | 16 | +1 | ⚠️ PAPER |
| `miss-14-15-6-2` | miss | 15 | 14 | -1 | ✅ CODE |
| `miss-14-30-6-4` | miss | 15 | 14 | -1 | ✅ CODE |
| `miss-15-15-7-1` | miss | 17 | 16 | -1 | ✅ CODE |
| `miss-15-15-7-4` | miss | 15 | 17 | +2 | ⚠️ PAPER |
| `miss-16-15-7-2` | miss | 17 | 16 | -1 | ✅ CODE |
| `miss-16-15-7-5` | miss | 18 | 17 | -1 | ✅ CODE |
| `miss-17-15-8-1` | miss | 17 | 20 | +3 | ⚠️ PAPER |
| `miss-17-15-8-2` | miss | 20 | 19 | -1 | ✅ CODE |
| `miss-17-15-8-3` | miss | 19 | 18 | -1 | ✅ CODE |
| `miss-17-15-8-4` | miss | 19 | 18 | -1 | ✅ CODE |
| `miss-17-30-8-4` | miss | 18 | 17 | -1 | ✅ CODE |

