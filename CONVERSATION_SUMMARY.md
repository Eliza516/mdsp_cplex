# Nhật ký làm việc dự án MDSP Solver (C++ / CPLEX)

**Thời gian**: 08-09/08/2026  
**Conversation ID**: `cd36856d-88c0-40db-9836-c8a948e9e8d0`  

---

## 📌 1. Kết nối Máy chủ từ xa (SSH & Tailscale)
- Khởi chạy và khôi phục dịch vụ Tailscale (`tailscale up`).
- Kết nối thành công SSH không cần mật khẩu tới máy chủ **orlab** (`orlab-machine-4` / `100.84.25.32`) với tài khoản `orlab`.

---

## 📌 2. Truyền nhận dữ liệu & Tương tác VS Code
- Chép hai thư mục dự án `mdsp` và `mdsp_cplex` từ `~/Downloads` sang máy `orlab` (`~/mdsp` và `~/mdsp_cplex`).
- Mở VS Code truy cập Remote-SSH trực tiếp tới thư mục mã nguồn từ xa.

---

## 📌 3. Đọc hiểu Bài báo & Cài đặt Thuật toán
**Bài báo nghiên cứu**: *"Improved Results for the Minimum Distance Superset Problem"* (Nguyễn Minh Hải, Nguyễn Ngọc Anh, Phạm Hương, Đỗ Đức Đông, Nguyễn Điệp, Nguyễn Minh Hoàng).

### Các thuật toán đã lập trình hoàn thiện:
1. **Thorn Arc Pruning (Section 4.1)**: Quy hoạch động Subset-Sum phát hiện và loại bỏ các khoảng cách "gai", giải trên tập thu nhỏ và tái cấu trúc $P^*$ theo Định lý 1.
2. **Cyclic Group Lower Bound (Section 4.2)**: Duyệt trên nhóm xoay $\mathbb{Z}/g\mathbb{Z}$ ($g \in \{2, 3, 5\}$) tính cận dưới $l$ chặt hơn theo Định lý 2.
3. **Large Neighborhood Search (LNS - Section 4.4)**: Thuật toán Construct & Destroy (Algorithm 4) thu hẹp cận trên $u$.
4. **Feasibility MILP $(\mathcal{P}_2)$ & Binary Search (Section 3 & Algorithm 1)**: Bộ giải tìm kiếm nhị phân FEAS over $t \in [l, u]$.

---

## 📌 4. Tổ chức lại Cấu trúc Mã nguồn (Clean Architecture)

```text
mdsp_cplex/
├── include/
│   ├── core/       # Types.h, Reader.h
│   ├── bounds/     # Bounds.h, ThornArcPruner.h, CyclicLowerBound.h, LNSUpperBound.h
│   ├── models/     # Model.h (P1), FeasModel.h (P2), BinarySearchSolver.h (FEAS)
│   └── runner/     # CsvExporter.h, BatchRunner.h
├── src/
│   ├── core/
│   ├── bounds/
│   ├── models/
│   ├── runner/
│   └── main.cpp
├── data/
├── Makefile
└── README.md
```

---

## 📌 5. Biên dịch & Kiểm thử Kết quả
- Tất cả các mã nguồn đã được biên dịch không lỗi trên máy `orlab`.
- Chạy nghiệm chính xác $P^* = \{0, 15, 26, 32, 36\}$ trên instance `full-5-15-1.txt` trong **0.09 giây**.
