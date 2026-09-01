 ### 1. Viết Unit Test (Test độc lập module)

  Bạn không nên chạy ngay toàn bộ luồng chương trình CPLEX. Hãy tách riêng
  hàm/class ThornArcPruner ra để test với các dữ liệu đầu vào cố định.

  • Test Case 1 (Dữ liệu từ bài báo):
  Tạo một bài test với mảng đầu vào D = {1,2,3,4,5,6,7,100}.
  Kiểm tra xem phương thức getThornArcs() có trả về chính xác [100] và
  getPrunedD() có trả về [1, 2, 3, 4, 5, 6, 7] hay không.
  • Test Case 2 (Trường hợp không có Thorn Arc):
  Tập D = {1,2,3,4,10} (vì 10 = 1 + 2 + 3 + 4).
  Phải đảm bảo kết quả trả về là tập rỗng [] và tập D giữ nguyên.
  • Test Case 3 (Trường hợp nhiều Thorn Arcs):
  Tập D = {1,2,3,100,200}.
  Kỳ vọng kết quả trả về [100, 200].
  • Test Case 4 (Trường hợp Edge Cases):
  Tập D chỉ có 1 phần tử (ví dụ: [5]), tập D có các khoảng cách giống hệt nhau
  ([2, 2, 2, 6]). Code không được văng lỗi (Exception).

  ### 2. Viết Integration Test (Test tính nhất quán với CPLEX)
  Khi Unit Test đã qua, bạn cần test xem việc áp dụng ThornArcPruner có làm
  thay đổi tính đúng đắn của toàn bộ mô hình CPLEX hay không.
  Bạn hãy chọn ra khoảng 5-10 bài toán mẫu (từ dữ liệu Fontoura chẳng hạn) và
  thiết lập kịch bản chạy song song:

  • Nhánh 1 (Không cắt tỉa): Đưa toàn bộ mảng D gốc vào cho mdsp_cplex giải ⇒
  Thu được giá trị điểm cực đại (Optimal Objective) là O₁.
  • Nhánh 2 (Có cắt tỉa):
      1. Chạy ThornArcPruner để lấy tập đã cắt D' và các gai Dₕ.
      2. Đưa D' vào mdsp_cplex giải ⇒ Thu được cấu hình điểm P'.
      3. Dùng Định lý 1 (Theorem 1 trong bài báo) để cộng nối lại:

     *
    P  = P' ∪ ⎛D  + p⎞
              ⎝ h    ⎠

  với p ∈ P'. Từ đó tính ra nghiệm O₂.

  • So sánh đối chiếu (Assertion): Kết quả của hệ thống chỉ đúng khi O₁ tương
  đương với O₂ (hoặc tập siêu khoảng cách sinh ra từ
     *
    P

  phải chứa đầy đủ tập D ban đầu).

drand-7-110-1 
drand-7-110-2 
drand-7-110-3 
drand-5-110-2
drand-5-110-3 

These are the results from the five requested files:

drand-7-110-1.txt
PRUNED: 12 20 32 36 50 78
THORN: 5
APPLIED: true

drand-7-110-2.txt
PRUNED: 11 20 21 65 94 95
THORN: 101
APPLIED: true

drand-7-110-3.txt
PRUNED: 8 21 59 72 88 110
THORN: 36
APPLIED: true
drand-5-110-2.txt

PRUNED: empty
THORN: 1 10 56 93 108
APPLIED: true
drand-5-110-3.txt

PRUNED: empty
THORN: 15 30 33 35 62
APPLIED: true

  ### 3. Cài đặt cờ (Flag) Toggle trong luồng code chính
  Trong file main hoặc hàm điều phối CPLEX, bạn nên bọc ThornArcPruner qua một
  cờ config (ví dụ: ENABLE_PRUNING = true/false).

    # Ví dụ logic trong file luồng chính (Main/Solver)
    if ENABLE_PRUNING:
        pruner = ThornArcPruner(D_input)
        thorn_arcs = pruner.get_thorn_arcs()
        D_to_solve = pruner.get_pruned_data()
    else:
        D_to_solve = D_input
        thorn_arcs = []
    
    # Đưa D_to_solve vào CPLEX Model
    cplex_model.build_model(D_to_solve)
    solution = cplex_model.solve()
    
    # Hậu xử lý (Post-processing)
    if ENABLE_PRUNING and len(thorn_arcs) > 0:
        solution = rebuild_solution_with_thorns(solution, thorn_arcs)

  Việc này giúp bạn nhanh chóng bật/tắt (Bật thì chạy nhanh hơn, Tắt thì chạy
  chậm nhưng chắc chắn an toàn) để debug xem CPLEX bị sai là do bản thân mô
  hình IP hay do ThornArcPruner lỡ tay xóa mất khoảng cách quan trọng.
  ### 4. Ghi Log chi tiết (Logging)
  Ở trong file ThornArcPruner, hãy in ra log (hoặc file log):
  [INFO] Kích thước D ban đầu: 100
  [INFO] Đã phát hiện 3 thorn arcs: [850, 920, 1000]. Bỏ qua DP.

## 2. Cận dưới (Lower Bound Cut).

  ### Bước 1: Test hàm chuyển đổi dữ liệu sang dạng Modulo g

  Trước khi giải thuật toán, mảng D gốc được biến đổi thành mảng
    D
     g
  theo công thức:
    d  = min (d pmod g,g - (d pmod g))
     g
  Bạn cần viết một test case riêng để đảm bảo mảng

    D
     g
  và việc đếm tần suất mⱼ là hoàn toàn chính xác.
  Test Case theo ví dụ bài báo (Trang 7):

  • Input: D = {1,2,3,4,5} và g = 3.
  • Kỳ vọng biến đổi:
      • 1 pmod 3 → min (1,3 - 1) = 1
      • 2 pmod 3 → min (2,3 - 2) = 1
      • 3 pmod 3 → min (0,3 - 0) = 0
      • 4 pmod 3 → min (1,3 - 1) = 1
      • 5 pmod 3 → min (2,3 - 2) = 1
  • Mảng đầu ra mong muốn: D₃ = {1,1,0,1,1}.
  • Tần suất mong muốn: Có một số 0 (m₀ = 1) và bốn số 1 (m₁ = 4).
  ### Bước 2: Test thuật toán Brute-force lõi (Algorithm 2)

  Bài báo mô tả thuật toán tìm
    n
     g
  nhỏ nhất (
    n  = x  + x  + x
     g    0    1    2

  ) thỏa mãn 2 bất phương trình (cho trường hợp g = 3):
  1. x₀(x₀ - 1) + x₁(x₁ - 1) + x₂(x₂ - 1) ≥ 2m₀
  2. x₀x₁ + x₁x₂ + x₂x₀ ≥ m₁

  Bạn hãy viết Test Case để truyền thẳng tần suất m₀,m₁ vào hàm Brute-force
  thay vì ném cả mảng D:
  Test Case:
  • Input: Truyền m₀ = 1 và m₁ = 4 vào hàm calculateNg_For_G3(m0, m1).
  • Tính toán bằng tay:
      • Nếu tổng x₀ + x₁ + x₂ = 3: Giá trị lớn nhất của x₀x₁ + x₁x₂ + x₂x₀ đạt
      được khi x₀ = 1,x₁ = 1,x₂ = 1 (tổng bằng 3). Mà m₁ = 4, nên điều kiện
      (2) sai. Nghĩa là n₃ không thể bằng 3.
      • Nếu tổng x₀ + x₁ + x₂ = 4: Giả sử x₀ = 2,x₁ = 1,x₂ = 1.
          • ĐK1: 2(1) + 1(0) + 1(0) = 2 ≥ 2m₀ (Đúng).
          • ĐK2: 2(1) + 1(1) + 1(2) = 5 ≥ m₁ (Đúng).

  • Output mong muốn: Hàm phải trả về n_3 = 4.

  ### Bước 3: Test hàm lấy Cận dưới cuối cùng (The final Lower Bound)

  Thuật toán hoàn chỉnh không chỉ chạy cho g = 3 mà chạy cho nhiều g khác
  nhau, cụ thể bài báo dùng g ∈ {2,3,5}. Cận dưới cuối cùng là L = max
  (n₂,n₃,n₅).

  Test Case Tích hợp (Integration Test):
    def test_final_lower_bound():
        D = [1, 2, 3, 4, 5]
        
        # Hàm compute_bound(D, g) bao gồm cả bước biến đổi mảng D -> D_g và
  chạy Brute-force
        n2 = compute_bound(D, g=2) 
        n3 = compute_bound(D, g=3) # Ta đã biết ở trên n3 = 4
        n5 = compute_bound(D, g=5)
        
        final_l = get_final_lower_bound(D, [2, 3, 5])
        
        # Kiểm tra xem final_l có thực sự trả về max(n2, n3, n5) hay không
        assert final_l == max(n2, n3, n5), "Lỗi: Không lấy giá trị Max giữa
  các g"
        
        # Kiểm tra chặn trên tầm thường (Trivial bound). 
        # Cận dưới lý thuyết không thể vượt qua Trivial Upper Bound = |D| + 1
        assert final_l <= len(D) + 1

  ### Bước 4: Kiểm tra vòng lặp vô hạn (Edge cases)

  Vì Algorithm 2 dùng tới 3 vòng lặp while lồng nhau (với g = 3) hoặc đệ quy
  lồng nhau với g = 5, rủi ro lớn nhất của file này là Bị treo (Infinite
  Loop). Bạn cần test các giới hạn an toàn:

  • Test với D là tập rỗng []: Kết quả phải bằng 1 hoặc 0, chương trình không
  được sập.
  • Test với D có các phần tử giống hệt nhau, ví dụ D = {10,10,10,10}.
  • Kiểm tra tính thoát lặp: Ensure rằng biến đếm x và điều kiện ngắt vòng lặp
  được lập trình đúng như Mã giả ở Bảng 2. Nếu thuật toán tìm thấy nghiệm (x₀,
  x₁,x₂) và cập nhật

    n
     g

  , các vòng lặp phải được tối ưu để chỉ xét đến

    x ,x  ≤ n
     0  1    g

  hiện tại.

  Nếu file của bạn pass qua được việc đếm tần suất

    D
     g

  và test ra đúng n_3 = 4 với tập {1,2,3,4,5}, bạn có thể tự tin 95% là hệ
  phương trình toán học của Lower Bound Cut đã được cài đặt chính xác!