# Báo cáo So sánh Kết quả Preprocessing

**Số lượng instances được so sánh:** 449 instances (Dữ liệu `full-*`).

## 1. Mức độ chặt của cận (Bound Tightness)

*(Ghi chú: Lower Bound (LB) càng lớn càng tốt, Upper Bound (UB) càng nhỏ càng tốt)*

| Tiêu chí | Code hiện tại tốt hơn | Paper tốt hơn | Bằng nhau |
|---|:---:|:---:|:---:|
| **Lower Bound (nLB)** | 15 | 5 | 429 |
| **Upper Bound Alg3 (nUB)** | 117 | 282 | 50 |
| **Upper Bound LNS (nDUB)** | **38** | 47 | 364 |

**Trung bình Upper Bound LNS (nDUB)**:
- Code hiện tại: `12.16`
- Trong Paper: `12.12`

## 2. Thời gian thực thi (Execution Time)

**Trung bình tổng thời gian chạy Preprocessing (ms)**: 
- Code hiện tại: `130.88 ms`
- Trong Paper: `1942.69 ms`

🚀 **Đánh giá:** Code hiện tại chạy nhanh hơn Paper trên **449 / 449** instances.
