Quick summary of the runtime flow (top → bottom) and where each piece lives.

Entry point

main() in main.cpp:1-400 — parses args, chooses batch vs single-instance mode and the runtime flags (--no-pruning, --no-cyclic, --no-lns, --algo).
Input loading

Single-instance: caller calls MDSPReader::readFromFile(path) (see Reader.cpp:1-120).
Reader reads all tokens, skips a leading NAME token if present, accepts an optional leading k count (it detects and strips it), then builds inst.D. It throws if any distance ≤ 0.
Main single-instance pipeline (order of operations)

currentInst ← originalInst (a copy of what the reader returned). main.cpp:54-74
Thorn-arc pruning (optional): ThornArcPruner::prune(originalInst) — if pruneRes.applied then update currentInst and save thornArcs. See ThornArcPruner in ThornArcPruner.h:1-40 and ThornArcPruner.cpp:1-220.
Purpose: identify/removes “thorn” distances that cannot be realized by any optimal point set; returns PruningResult with prunedInstance, thornArcs, and applied.
Compute trivial bounds: MDSPBoundsCalculator::computeTrivialBounds(currentInst) (see Bounds.cpp:1-80).
Produces initial lower bound l, upper bound u and a derived B.
Cyclic Z/gZ lower bound (optional): CyclicLowerBound::computeLowerBound(currentInst) (see CyclicLowerBound.cpp:1-200).
If this l_cyclic > bounds.l it replaces the global lower bound.
Upper-bound improvements (optional, --no-lns controls):
Arithmetic-sequence construction: ArithSequenceUB::construct(currentInst, bounds.B) to propose an improved u (see ArithSequenceUB.cpp:1-240).
LNS destroy-and-repair: LNSUpperBound::solve(currentInst, bounds.B, iterations) to try to further reduce u (see LNSUpperBound.cpp:1-240).
Final refinement of B: MDSPBoundsCalculator::refineValueBound(currentInst, bounds.u) (see Bounds.cpp:1-80).
Solver selection:
If --algo feas → BinarySearchSolver::solve(currentInst, bounds.l, bounds.u, bounds.B, timeLimit, true) (see BinarySearchSolver.cpp:1-200).
Binary search repeatedly builds a feasibility model (FeasModel) for candidate sizes t and queries CPLEX to test feasibility.
Else (default p1) → instantiate MDSPModel(currentInst, bounds.l, bounds.u, bounds.B, timeLimit) and call MDSPModel::solve(...) (integer programming, builds CPLEX model, see Model.cpp:1-400).
Post-processing (if pruning was applied): ThornArcPruner::reconstructPoints(sol.points, thornArcs) to re-insert points for thorn arcs and update sol.objective (see ThornArcPruner.cpp:1-220).
Print final outputs: status, LB, UB, gap, P*, CPLEX time and wall time. (See end of main in main.cpp:200-400.)
Batch mode

If program started with --batch, BatchRunner::runDirectory(dir) handles iterating files and running the same pipeline per instance (see BatchRunner.cpp:1-140 and [include/runner/BatchRunner.h]).
Flags & control points

--no-pruning disables ThornArcPruner::prune(...).
--no-cyclic disables the cyclic lower-bound step.
--no-lns disables ArithSequence and LNS upper-bound improvements.
--algo p1|feas chooses the solver path (IP model vs binary-search feasibility).
Time limits are passed to the solver (CPLEX) from main

 Bạn hãy giúp tôi biên dịch và chạy chương trình giải bài toán Minimum
  Distance Superset Problem (MDSP) viết bằng C++ (sử dụng thư viện CPLEX).


    Nhiệm vụ của bạn:

    [Bước 1] - Compile (Biên dịch):
    Hãy kiểm tra xem dự án sử dụng CMake hay Makefile và thực hiện build
  project ra file thực thi (ví dụ: `mdsp_solver`). Đảm bảo link đúng với thư
  viện CPLEX.

    [Bước 2] -  "Hãy chạy thử chương trình trên toàn bộ thư mục mdsp_instances" với 2 cấu hình thuật toán khác nhau để đối chiếu
  kết quả:
    - Cấu hình 1 (Chạy mô hình MIP): Chạy file thực thi với cờ `--algo p1`.
    - Cấu hình 2 (Chạy Binary Search): Chạy file thực thi với cờ `--algo feas`.

    [Bước 3] - Run Ablation Study (Test hiệu quả của tiền xử lý):
    Hãy chạy lại lệnh trên file dữ liệu mẫu nhưng tắt lần lượt các tính năng
  tiền xử lý để xem thời gian chạy (runtime) thay đổi thế nào. Hãy chạy các
  lệnh sau và phân tích log đầu ra:
    - Lệnh 1: `--algo feas` (Bật toàn bộ tính năng mặc định)
    - Lệnh 2: `--algo feas --no-pruning` (Tắt Thorn-arc pruning)
    - Lệnh 3: `--algo feas --no-cyclic` (Tắt Cyclic Lower Bound)
    - Lệnh 4: `--algo feas --no-lns` (Tắt LNS Upper Bound)

    [Bước 4] - Báo cáo kết quả:
    Sau khi chạy xong, hãy tóm tắt lại cho tôi dưới dạng bảng hoặc danh sách
  gạch đầu dòng các thông tin sau từ Terminal log:
    - Tên Instance
    - Thuật toán / Các cờ đã dùng
    - Cận dưới (LB) và Cận trên (UB) trước khi CPLEX chạy.
    - Nghiệm tối ưu tìm được (Kích thước tập P*)
    - Tổng thời gian chạy (Wall time / CPLEX time)

    Nếu trong quá trình build hoặc run có bất kỳ lỗi nào (Lỗi đường dẫn CPLEX,
  Segmentation Fault, v.v.), hãy báo lại nguyên nhân và đề xuất cách fix cho
  tôi.
  

