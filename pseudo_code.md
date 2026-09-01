## Thorn Arc Prunning

Function PruningPhase(D):
    // Đầu vào: Tập hợp đa (multiset) các khoảng cách D
    // Đầu ra: D_pruned (tập D sau khi đã loại bỏ thorn arcs) và D_h (tập các thorn arcs)
    
    // Điều kiện áp dụng từ bài báo: Số lượng khoảng cách < 1000 và khoảng cách lớn nhất <= 1000
    If |D| >= 1000 OR Max(D) > 1000:
        Return D, {} 
        
    D_h = {} // Khởi tạo tập hợp chứa các thorn arcs
    
    For each d in D:
        D_rem = D \ {d} // Tập D loại bỏ phần tử d
        
        // Tạo mảng W chứa các phần tử của D_rem với hệ số +1 và -1
        W = D_rem U {-x | x in D_rem}
        Let W be indexed as {w_1, w_2, ..., w_n} where n = |W|
        
        // Tính toán cận A (tổng âm) và B (tổng dương)
        B = Sum(x for x in D_rem)
        A = -B 
        
        // Khởi tạo bảng quy hoạch động Q kích thước [1..n][A..B] với giá trị False
        // Q(i, x) mang ý nghĩa: có tồn tại tập con của {w_1..w_i} có tổng bằng x hay không
        Initialize Q[1..n][A..B] to False
        
        // Trường hợp cơ sở (Base case) cho i = 1
        For x from A to B:
            If w_1 == x:
                Q[1][x] = True
            // Ghi chú thêm: Trong Subset Sum chuẩn, tổng của tập rỗng = 0 nên Q[1][0] cũng là True.
            // Nếu x == 0, Q[1][x] = True có thể cần thiết để thuật toán chạy đúng thực tế.
                
        // Bước đệ quy (Recursion) cho i từ 2 đến n
        For i from 2 to n:
            For x from A to B:
                term1 = Q[i-1][x]
                term2 = False
                If (x - w_i) >= A AND (x - w_i) <= B:
                    term2 = Q[i-1][x - w_i]
                    
                // Q(i, x) = Q(i - 1, x) v Q(i - 1, x - w_i)
                Q[i][x] = term1 OR term2 
                
        // Kiểm tra xem d có phải là thorn arc không
        // Nếu không có tập con nào có tổng bằng d (Q[n][d] == False), thì d là thorn arc
        If Q[n][d] == False:
            D_h = D_h U {d}
            
    // Loại bỏ các thorn arcs khỏi D ban đầu
    D_pruned = D \ D_h
    
    Return D_pruned, D_h

## Algorithm 2: Brute-force MDSP_g for g = 3

Hàm computeLowerBound(inst):
    // Nếu không có phần tử nào trong D, kết quả bằng 0
    Nếu inst.D rỗng: 
        Trả về 0

    // Tính toán cận dưới ứng với các chu kỳ g = 2, 3, và 5
    n2 = computeForG(inst, 2)
    n3 = computeForG(inst, 3)
    n5 = computeForG(inst, 5)

    // Lấy giá trị cận dưới lớn nhất
    Trả về MAX(n2, n3, n5)

---------------------------------------------------------

Hàm checkConstraints(x, g, m):
    // 1. Kiểm tra điều kiện tự kết hợp (j = 0)
    sumSame = 0
    Vòng lặp i từ 0 đến g - 1:
        sumSame = sumSame + x[i] * (x[i] - 1)
    
    Nếu sumSame < 2 * m[0]:
        Trả về False

    // 2. Kiểm tra điều kiện lệch pha (j từ 1 đến g/2)
    Vòng lặp j từ 1 đến phần_nguyên(g / 2):
        sumDiff = 0
        Vòng lặp i từ 0 đến g - 1:
            sumDiff = sumDiff + x[i] * x[(i + j) mod g]
        
        // Cập nhật yêu cầu (tránh đếm lặp 2 lần khi g chẵn và j = g/2)
        Nếu g là số chẵn VÀ j == g / 2:
            required = 2 * m[j]
        Ngược lại:
            required = m[j]
            
        Nếu sumDiff < required:
            Trả về False

    Trả về True

---------------------------------------------------------

Hàm computeForG(inst, g):
    // 1. Phân tích tần suất các khoảng cách d theo chu kỳ g
    Khởi tạo mảng m có kích thước (g/2 + 1) chứa toàn số 0
    
    Vòng lặp cho mỗi phần tử d trong inst.D:
        modVal = d mod g
        Nếu modVal < 0:
            modVal = modVal + g
            
        d_g = MIN(modVal, g - modVal)
        
        Nếu d_g <= g / 2:
            m[d_g] = m[d_g] + 1

    // 2. Tìm N nhỏ nhất (số điểm) thỏa mãn hệ thống
    maxPoints = kích thước của inst.D + 1
    
    Vòng lặp N từ 1 đến maxPoints:
        Khởi tạo mảng x kích thước g chứa toàn số 0
        found = False

        // Định nghĩa hàm đệ quy để sinh các cấu hình tổng bằng N
        Hàm đệ quy searchCombination(idx, remainingSum):
            Nếu found == True:
                Thoát (return)
                
            // Base case: Nếu xét đến phần tử cuối cùng
            Nếu idx == g - 1:
                x[idx] = remainingSum
                Nếu checkConstraints(x, g, m) == True:
                    found = True
                Thoát (return)
                
            // Thử gán các giá trị có thể có cho phần tử hiện tại
            Vòng lặp val từ 0 đến remainingSum:
                x[idx] = val
                Gọi searchCombination(idx + 1, remainingSum - val)
                Nếu found == True:
                    Thoát (return)

        // Bắt đầu tìm kiếm tổ hợp từ chỉ số 0 và tổng N
        Gọi searchCombination(0, N)
        
        // Nếu tìm thấy một cấu hình x hợp lệ, N chính là kết quả nhỏ nhất
        Nếu found == True:
            Trả về N

    Trả về maxPoints

## Hàm 4.3 computeMultiplicationUpperBound(D):
    Nếu D rỗng:
        Trả về 0

    // ===== Bước 1: Tách D1 = tập giá trị PHÂN BIỆT của D =====
    D1 = SET(D)

    // ===== Bước 2: Tính r, s theo max(D1) =====
    r = ceil( sqrt( (MAX(D1) + 1) / 2 ) )
    s = r + 1

    // ===== Bước 3: Xây dựng tập X = {i*r + j*s | 0 < i ≤ s, 0 < j ≤ r} =====
    X = {}
    Vòng lặp i từ 1 đến s:
        Vòng lặp j từ 1 đến r:
            X.add(i*r + j*s)
    // |X| = s * r ≈ max(D1)/2  (đúng như bài báo chứng minh)

    // ===== Bước 4: D1X = phần tử của D1 biểu diễn được dưới dạng i*r + j*s =====
    D1X = D1 ∩ X

    // Trích xuất R (tập các hệ số i) và S (tập các hệ số j) xuất hiện trong D1X
    R = {}
    S = {}
    Với mỗi d trong D1X:
        Tìm (i, j) DUY NHẤT sao cho d = i*r + j*s     // bài báo chứng minh biểu diễn này duy nhất nếu tồn tại
        R.add(i)
        S.add(j)

    // ===== Bước 5: Giải MDSP trên 2 bài toán con NHỎ HƠN NHIỀU: R và S =====
    // (|R| << |D|, |S| << |D|, giá trị các phần tử R, S nhỏ hơn D theo hệ số √max(D)/2
    //  → chi phí giải MDSP trên R, S là KHÔNG ĐÁNG KỂ so với giải trực tiếp trên D)
    PR = SolveMDSP(R)
    PS = SolveMDSP(S)

    // ===== Bước 6: Xây dựng tập ứng viên A bằng cách "biến đổi ngược" qua r, s =====
    A = {}
    Với mỗi pi trong PR:
        Với mỗi pj trong PS:
            A.add(pi * r + pj * s)

    // ===== Bước 7: Dùng thuật toán tham lam (Algorithm 3) chọn P1X ⊆ A =====
    // sao cho P1X là 1 distance superset của D1X, |P1X| nhỏ nhất có thể
    P1X = greedyConstructP1X(A, D1X)

    // ===== Bước 8: Phần còn lại D2 = D \ Δ(P1X) =====
    // (Δ(P1X): multiset TOÀN BỘ khoảng cách đôi một sinh bởi P1X)
    D2 = D \ Δ(P1X)      // phép trừ multiset

    // ===== Bước 9: Cận trên mới =====
    Trả về |P1X| + |D2|


Hàm greedyConstructP1X(A, D1X):        // Algorithm 3
    remaining = bản sao multiset của D1X
    P1X = {}

    Trong khi remaining KHÔNG rỗng:
        bestElement = None
        bestCoverCount = -1

        Với mỗi x trong A:
            // "x - P1X" = {x - y | y ∈ P1X}; nếu P1X rỗng, coi như điểm gốc {0} ⇒ diffs = {x}
            diffs = { x - y  với mỗi y trong P1X } ∪ (nếu P1X rỗng thì {x})

            coverCount = số phần tử của diffs khớp với remaining (tính cả trùng lặp)

            Nếu coverCount > bestCoverCount:
                bestCoverCount = coverCount
                bestElement = x

        // "pop" phần tử đóng góp nhiều khoảng cách nhất, thêm vào P1X
        P1X.add(bestElement)
        A.remove(bestElement)

        Với mỗi y trong P1X (y != bestElement):
            diff = |bestElement - y|
            Nếu diff thuộc remaining:
                remaining.remove(diff)    // xóa 1 lần xuất hiện (multiset)

    Trả về P1X


## 4. Large Neighborhood Search (LNS) for MDSP - Updated C++ Version

Input : D          - Tập hợp khoảng cách mục tiêu (multiset)
        B          - Biên trên của tọa độ các điểm [0, B]
        iterations - Số vòng lặp destroy-and-repair (t)

Output: bestP      - Tập điểm P nhỏ nhất phủ hoàn toàn D

----------------------------------------------------------------
FUNCTION SOLVE(D, B, iterations):
    If D is empty: Return ∅
    Initialize RNG with seed 42

    // --- Phase 1: Initial Construction ---
    currentP ← {0}
    GREEDY_REPAIR(currentP, D, B, RNG)
    bestP ← currentP

    // --- Phase 2: Destroy-and-Repair Loop ---
    For iter = 1 .. iterations:
        If |bestP| ≤ 2: Break

        // --- Step 2a: Constructive Greedy Destroy ---
        b ← max(1, ⌊0.1 × |currentP|⌋)
        Q ← ∅
        If 0 ∈ currentP: Q ← {0}
        
        uncoveredMap ← FrequencyMap(D)

        While |Q| < b:
            bestCandidates ← ∅
            maxGain ← -1

            For x ∈ currentP \ Q:
                gain ← 0
                For q ∈ Q:
                    dist ← |x - q|
                    If dist ∈ uncoveredMap and count(dist) > 0:
                        gain ← gain + 1

                If gain > maxGain:
                    maxGain ← gain
                    bestCandidates ← {x}
                Else If gain == maxGain and gain > -1:
                    bestCandidates ← bestCandidates ∪ {x}

            If bestCandidates is empty: Break

            // Tie-breaking: Chọn ngẫu nhiên 1 điểm tốt nhất
            bestX ← RandomChoice(bestCandidates, RNG)

            // Cập nhật khoảng cách đã bị phủ bởi bestX với các điểm trong Q
            For q ∈ Q:
                dist ← |bestX - q|
                If dist ∈ uncoveredMap and count(dist) > 0:
                    uncoveredMap[dist] ← uncoveredMap[dist] - 1

            Q ← Q ∪ {bestX}

        currentP ← SORT(Q)

        // --- Step 2b: Randomized Greedy Repair ---
        GREEDY_REPAIR(currentP, D, B, RNG)

        // --- Step 2c: Acceptance Criterion ---
        If |currentP| < |bestP|:
            bestP ← currentP

    Return SORT(bestP)

----------------------------------------------------------------
FUNCTION GREEDY_REPAIR(P, D, B, RNG):
    While True:
        deltaP ← ComputePairwiseDistances(P)
        uncoveredD ← D \ deltaP          // Phép trừ đa tập hợp (multiset)

        If uncoveredD is empty: Break    // Đã phủ hoàn toàn D

        // 1. Sinh tập ứng viên thu hẹp (p ± d)
        candidates ← ∅
        For p ∈ P:
            For d ∈ uncoveredD:
                If p + d ≤ B: candidates ← candidates ∪ {p + d}
                If p - d ≥ 0: candidates ← candidates ∪ {p - d}
        candidates ← UniqueSorted(candidates) \ P

        // 2. Đánh giá độ lợi (Gain) của từng ứng viên
        bestCandidates ← ∅
        maxGain ← -1

        For x ∈ candidates:
            gain ← 0
            tempUncovered ← FrequencyMap(uncoveredD)
            For p ∈ P:
                dist ← |x - p|
                If dist ∈ tempUncovered and count(dist) > 0:
                    gain ← gain + 1
                    tempUncovered[dist] ← tempUncovered[dist] - 1

            If gain > maxGain:
                maxGain ← gain
                bestCandidates ← {x}
            Else If gain == maxGain and gain > -1:
                bestCandidates ← bestCandidates ∪ {x}

        // 3. Lựa chọn điểm bổ sung (Phá hòa ngẫu nhiên)
        bestX ← -1
        If maxGain > 0 and bestCandidates is not empty:
            bestX ← RandomChoice(bestCandidates, RNG)
        Else:
            // Fallback nếu không có ứng viên nào tạo ra gain > 0
            bestX ← min(P.last() + uncoveredD.first(), B)

        P ← P ∪ {bestX}


## 4.5
Hàm computeTrivialBounds(inst):
    Khởi tạo đối tượng b (chứa các thuộc tính l, u, B)
    k = số lượng phần tử của inst (inst.k())

    // 1. Tính cận dưới l0 = ceil( 1/2 + sqrt(1/4 + 2k) )
    raw = 0.5 + căn_bậc_hai(0.25 + 2.0 * k)
    // Trừ đi 1e-9 để xử lý sai số của số thực (floating-point) trước khi làm tròn lên
    b.l = làm_tròn_lên(raw - 1e-9) 

    // 2. Tính cận trên u0
    b.u = k + 1

    // 3. Làm chặt giới hạn B (dựa trên Định lý 3)
    b.B = refineValueBound(inst, b.u)
    
    Trả về b

---------------------------------------------------------

Hàm refineValueBound(inst, u):
    // Nếu tập hợp D rỗng, không có giới hạn nào cả
    Nếu inst.D rỗng:
        Trả về 0

    // 1. Chuẩn bị mảng và sắp xếp
    Tạo mảng sorted là bản sao của inst.D
    Sắp xếp mảng sorted theo thứ tự tăng dần

    dmin = sorted[0]                   // Phần tử nhỏ nhất
    dmax = sorted[cuối_cùng]           // Phần tử lớn nhất
    n = kích thước của mảng sorted

    // Xác định số lượng phần tử cần lấy để tính tổng (không vượt quá n)
    uu = MIN(u, n)

    // 2. Tính tổng của uu phần tử lớn nhất trong mảng
    sumDu = 0
    Vòng lặp i từ (n - uu) đến (n - 1):
        sumDu = sumDu + sorted[i]

    // 3. Tính toán ứng viên giới hạn
    // Lưu ý: Phép chia 2 ở đây là chia lấy phần nguyên (integer division)
    candidate = phần_nguyên((sumDu - dmin) / 2)

    // 4. Giới hạn giá trị B không được nhỏ hơn phần tử lớn nhất trong mảng (dmax)
    Trả về MAX(candidate, dmax)