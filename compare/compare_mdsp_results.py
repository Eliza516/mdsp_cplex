"""
compare_mdsp_results.py
========================

So sanh ket qua thuc nghiem cua 2 nguon du lieu cho bai toan MDSP
(Minimum Distance Superset Problem):

    - Nguon A: file .rtf (bang ket qua dang "paper table") voi toi da
      9 phuong phap: nIP, nFEAS, nMAX, IP, FEAS, MAX, tIP, tFEAS, tMAX
      (moi phuong phap co 4 chi so: LB, UB, gap, time)
    - Nguon B: file .csv voi cac cot dang "<method>_LB/UB/gap/time"
      (hien tai chi co 6/9 phuong phap: nIP, nFEAS, IP, FEAS, tIP, tFEAS)

Thiet ke de "chiu duoc" viec 2 file khong co cung tap phuong phap /
cung tap instance:
    - Neu 1 phuong phap chi xuat hien o 1 nguon        -> bao cao rieng,
      khong ep so sanh (sau nay CSV co them cot la se tu dong duoc
      dua vao phan so sanh hai-nguon ma khong can sua code).
    - Neu 1 instance chi xuat hien o 1 nguon            -> liet ke rieng
      trong "coverage report", khong tinh vao cac chi so so sanh.
    - CSV co the co instance bi lap (chay lai nhieu lan) -> tu dong
      gom nhom va lay trung binh, co canh bao so lan lap.

Cach dung
---------
    python3 compare_mdsp_results.py <file_A.rtf> <file_B.csv> \
        --name-a "RTF (paper)" --name-b "CSV (rerun)" \
        --outdir ./output

Dau ra
------
    - comparison_details.csv : bang chi tiet, 1 dong / (instance, method)
    - comparison_summary.md  : bao cao tong hop kieu so sanh 2 paper
"""

from __future__ import annotations

import argparse
import csv
import math
import re
import statistics
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# ---------------------------------------------------------------------------
# Cau hinh chung
# ---------------------------------------------------------------------------

# Gioi han thoi gian chay (giay) dung de xac dinh 1 lan chay co "timeout"
# hay khong. Chinh lai neu paper dung gioi han khac.
TIME_LIMIT = 3600.0

# Ten day du cua tung phuong phap, chi de hien thi cho dep trong bao cao.
# Neu file co them phuong phap moi khong co trong dict nay, script van
# chay binh thuong, chi la khong co ten day du.
METHOD_LABELS = {
    "nIP": "Node-count IP (exact)",
    "nFEAS": "Node-count Feasibility heuristic",
    "nMAX": "Node-count Max-bound",
    "IP": "Integer Programming (exact)",
    "FEAS": "Feasibility heuristic",
    "MAX": "Max-bound heuristic",
    "tIP": "Time-limited IP",
    "tFEAS": "Time-limited Feasibility heuristic",
    "tMAX": "Time-limited Max-bound",
}


@dataclass
class Record:
    """1 ket qua (instance, method) -> LB / UB / gap / time."""
    lb: Optional[float] = None
    ub: Optional[float] = None
    gap: Optional[float] = None
    time: Optional[float] = None
    n_runs: int = 1  # so lan da gom trung binh (duplicate) tu nguon


# instance -> |D| -> ...  và instance -> method -> Record
@dataclass
class ResultSet:
    name: str
    sizeD: Dict[str, float] = field(default_factory=dict)
    data: Dict[str, Dict[str, Record]] = field(default_factory=dict)  # data[instance][method]
    duplicate_log: Dict[str, int] = field(default_factory=dict)

    def methods(self) -> List[str]:
        s = set()
        for m in self.data.values():
            s.update(m.keys())
        return sorted(s)

    def instances(self) -> List[str]:
        return sorted(self.data.keys())


# ---------------------------------------------------------------------------
# 1. Parser cho file RTF (bang ket qua kieu paper)
# ---------------------------------------------------------------------------

def _rtf_to_plain_text(raw: str) -> str:
    """Bo control-word cua RTF, giu lai phan text thuan."""
    text = raw.replace("\\\n", " ")          # line-continuation trong RTF
    text = re.sub(r"\\'[0-9a-fA-F]{2}", " ", text)   # ky tu hex-escape
    text = re.sub(r"\\[A-Za-z]+-?\d*[ ]?", " ", text)  # control words
    text = text.replace("{", " ").replace("}", " ")
    text = text.replace("\\", " ")
    return re.sub(r"\s+", " ", text).strip()


def parse_rtf(path: str, name: str) -> ResultSet:
    raw = Path(path).read_text(encoding="utf-8", errors="ignore")
    text = _rtf_to_plain_text(raw)

    header_match = re.search(
        r"Instances\s*\|D\|\s*(.*?)\s*LB\s+UB\s+gap\s+time", text
    )
    if not header_match:
        raise ValueError(f"[{path}] Khong tim thay dong header 'Instances |D| ...'")
    methods = header_match.group(1).split()
    n_methods = len(methods)

    rs = ResultSet(name=name)

    # Moi dong du lieu: "<instance> <|D|> " + n_methods * 4 con so
    row_re = re.compile(
        r"(full-\S+?)\s+([\d.]+)((?:\s+[\d.]+){" + str(n_methods * 4) + r"})"
        r"(?=\s+full-\S+|\s*$)"
    )
    for m in row_re.finditer(text):
        instance = m.group(1)
        d_val = float(m.group(2))
        nums = [float(x) for x in m.group(3).split()]
        rs.sizeD[instance] = d_val
        rs.data.setdefault(instance, {})
        for i, method in enumerate(methods):
            lb, ub, gap, t = nums[i * 4: i * 4 + 4]
            rs.data[instance][method] = Record(lb=lb, ub=ub, gap=gap, time=t)

    return rs


# ---------------------------------------------------------------------------
# 2. Parser cho file CSV
# ---------------------------------------------------------------------------

def parse_csv(path: str, name: str) -> ResultSet:
    rs = ResultSet(name=name)
    # instance -> method -> list ban ghi (de gom trung binh neu bi lap)
    buckets: Dict[str, Dict[str, List[Record]]] = defaultdict(lambda: defaultdict(list))
    d_vals: Dict[str, List[float]] = defaultdict(list)

    with open(path, newline="", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        # Tim tat ca ten phuong phap dua tren hau to _LB/_UB/_gap/_time
        methods = sorted({
            col[:-3] for col in reader.fieldnames or [] if col.endswith("_LB")
        })
        for row in reader:
            instance = row.get("Instances", row.get("Instance", "")).strip()
            if instance.lower().endswith(".txt"):
                instance = instance[:-4]
            if not instance:
                continue
            try:
                d_vals[instance].append(float(row["|D|"]))
            except (KeyError, ValueError):
                pass
            for method in methods:
                try:
                    lb = float(row[f"{method}_LB"])
                    ub = float(row[f"{method}_UB"])
                    gap = float(row[f"{method}_gap"])
                    t = float(row[f"{method}_time"])
                except (KeyError, ValueError):
                    continue
                buckets[instance][method].append(Record(lb=lb, ub=ub, gap=gap, time=t))

    for instance, methoddict in buckets.items():
        rs.data[instance] = {}
        n_dup = max((len(v) for v in methoddict.values()), default=1)
        if n_dup > 1:
            rs.duplicate_log[instance] = n_dup
        for method, records in methoddict.items():
            rs.data[instance][method] = Record(
                lb=statistics.mean(r.lb for r in records),
                ub=statistics.mean(r.ub for r in records),
                gap=statistics.mean(r.gap for r in records),
                time=statistics.mean(r.time for r in records),
                n_runs=len(records),
            )
        if instance in d_vals:
            rs.sizeD[instance] = d_vals[instance][0]

    return rs


# ---------------------------------------------------------------------------
# 3. Ghep 2 nguon lai theo (instance, method)
# ---------------------------------------------------------------------------

def build_details(rs_a: ResultSet, rs_b: ResultSet) -> List[dict]:
    all_instances = sorted(set(rs_a.instances()) | set(rs_b.instances()))
    all_methods = sorted(set(rs_a.methods()) | set(rs_b.methods()))

    rows = []
    for inst in all_instances:
        d_val = rs_a.sizeD.get(inst, rs_b.sizeD.get(inst))
        for method in all_methods:
            ra = rs_a.data.get(inst, {}).get(method)
            rb = rs_b.data.get(inst, {}).get(method)
            row = {
                "Instance": inst,
                "|D|": d_val,
                "Method": method,
                f"{rs_a.name}_LB": ra.lb if ra else None,
                f"{rs_a.name}_UB": ra.ub if ra else None,
                f"{rs_a.name}_gap": ra.gap if ra else None,
                f"{rs_a.name}_time": ra.time if ra else None,
                f"{rs_b.name}_LB": rb.lb if rb else None,
                f"{rs_b.name}_UB": rb.ub if rb else None,
                f"{rs_b.name}_gap": rb.gap if rb else None,
                f"{rs_b.name}_time": rb.time if rb else None,
            }
            if ra and rb:
                row["delta_UB"] = rb.ub - ra.ub
                row["delta_time"] = rb.time - ra.time
                row["present_in"] = "both"
            elif ra:
                row["delta_UB"] = None
                row["delta_time"] = None
                row["present_in"] = rs_a.name
            elif rb:
                row["delta_UB"] = None
                row["delta_time"] = None
                row["present_in"] = rs_b.name
            else:
                continue
            rows.append(row)
    return rows


def write_details_csv(rows: List[dict], out_path: Path, rs_a: ResultSet, rs_b: ResultSet):
    if not rows:
        return
    fieldnames = [
        "Instance", "|D|", "Method",
        f"{rs_a.name}_LB", f"{rs_a.name}_UB", f"{rs_a.name}_gap", f"{rs_a.name}_time",
        f"{rs_b.name}_LB", f"{rs_b.name}_UB", f"{rs_b.name}_gap", f"{rs_b.name}_time",
        "delta_UB", "delta_time", "present_in",
    ]
    with open(out_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


# ---------------------------------------------------------------------------
# 4. Cac chi so so sanh kieu "so sanh 2 paper hoc thuat"
# ---------------------------------------------------------------------------

def shifted_geomean(values: List[float], shift: float = 1.0) -> Optional[float]:
    """Shifted geometric mean - chuan trong benchmark MILP (vd. MIPLIB)."""
    vals = [v for v in values if v is not None]
    if not vals:
        return None
    logs = [math.log(v + shift) for v in vals]
    return math.exp(sum(logs) / len(logs)) - shift


def method_stats(rs: ResultSet, method: str) -> dict:
    gaps, times, solved, timeouts = [], [], 0, 0
    n = 0
    for inst, methods in rs.data.items():
        rec = methods.get(method)
        if rec is None:
            continue
        n += 1
        gaps.append(rec.gap)
        times.append(rec.time)
        if rec.gap is not None and rec.gap <= 1e-9:
            solved += 1
        if rec.time is not None and rec.time >= TIME_LIMIT:
            timeouts += 1
    return {
        "n_instances": n,
        "n_solved_optimal": solved,
        "pct_solved_optimal": (100.0 * solved / n) if n else None,
        "n_timeouts": timeouts,
        "avg_gap_pct": statistics.mean(gaps) if gaps else None,
        "avg_time_s": statistics.mean(times) if times else None,
        "shifted_geomean_time_s": shifted_geomean(times),
        "median_time_s": statistics.median(times) if times else None,
    }


def head_to_head(rs_a: ResultSet, rs_b: ResultSet, method: str) -> dict:
    """So sanh truc tiep tren cac instance ca 2 nguon deu co cho method nay."""
    wins_a = wins_b = ties = 0
    common_gap_diffs, common_time_diffs = [], []
    n_common = 0
    for inst in set(rs_a.instances()) & set(rs_b.instances()):
        ra = rs_a.data.get(inst, {}).get(method)
        rb = rs_b.data.get(inst, {}).get(method)
        if ra is None or rb is None:
            continue
        n_common += 1
        # "thang" duoc dinh nghia theo: gap thap hon truoc, roi den time thap hon
        if ra.gap < rb.gap - 1e-9:
            wins_a += 1
        elif rb.gap < ra.gap - 1e-9:
            wins_b += 1
        elif ra.time < rb.time - 1e-9:
            wins_a += 1
        elif rb.time < ra.time - 1e-9:
            wins_b += 1
        else:
            ties += 1
        common_gap_diffs.append(rb.gap - ra.gap)
        common_time_diffs.append(rb.time - ra.time)
    return {
        "n_common_instances": n_common,
        f"wins_{rs_a.name}": wins_a,
        f"wins_{rs_b.name}": wins_b,
        "ties": ties,
        "avg_gap_diff_B_minus_A": statistics.mean(common_gap_diffs) if common_gap_diffs else None,
        "avg_time_diff_B_minus_A": statistics.mean(common_time_diffs) if common_time_diffs else None,
    }


# ---------------------------------------------------------------------------
# 5. Sinh bao cao Markdown
# ---------------------------------------------------------------------------

def fmt(v, nd=2):
    if v is None:
        return "-"
    if isinstance(v, float):
        return f"{v:.{nd}f}"
    return str(v)


def write_summary_md(rs_a: ResultSet, rs_b: ResultSet, out_path: Path):
    inst_a, inst_b = set(rs_a.instances()), set(rs_b.instances())
    meth_a, meth_b = set(rs_a.methods()), set(rs_b.methods())
    common_methods = sorted(meth_a & meth_b)
    only_a_methods = sorted(meth_a - meth_b)
    only_b_methods = sorted(meth_b - meth_a)

    lines = []
    lines.append(f"# Bao cao so sanh ket qua MDSP: {rs_a.name} vs {rs_b.name}\n")

    # --- Coverage ---
    lines.append("## 1. Do phu du lieu (coverage)\n")
    lines.append(f"- So instance trong **{rs_a.name}**: {len(inst_a)}")
    lines.append(f"- So instance trong **{rs_b.name}**: {len(inst_b)}")
    lines.append(f"- So instance chung: {len(inst_a & inst_b)}")
    only_a_i = sorted(inst_a - inst_b)
    only_b_i = sorted(inst_b - inst_a)
    if only_a_i:
        lines.append(f"- Chi co o {rs_a.name} ({len(only_a_i)}): {', '.join(only_a_i)}")
    if only_b_i:
        lines.append(f"- Chi co o {rs_b.name} ({len(only_b_i)}): {', '.join(only_b_i)}")
    if rs_b.duplicate_log:
        lines.append(
            f"- **Canh bao**: {len(rs_b.duplicate_log)} instance trong {rs_b.name} "
            f"bi lap du lieu (nhieu lan chay), da tu dong lay trung binh: "
            + ", ".join(f"{k} (x{v})" for k, v in rs_b.duplicate_log.items())
        )
    lines.append("")
    lines.append(f"- Phuong phap chung ({len(common_methods)}): {', '.join(common_methods) or '(khong co)'}")
    if only_a_methods:
        lines.append(f"- Phuong phap chi co o {rs_a.name} ({len(only_a_methods)}): {', '.join(only_a_methods)} "
                      f"— *chua so sanh duoc, se tu dong duoc dua vao muc 3 khi {rs_b.name} co them cot tuong ung*")
    if only_b_methods:
        lines.append(f"- Phuong phap chi co o {rs_b.name} ({len(only_b_methods)}): {', '.join(only_b_methods)}")
    lines.append("")

    # --- Per-source, per-method stats ---
    lines.append("## 2. Chi so hieu nang tung nguon (moi phuong phap)\n")
    lines.append("Cac chi so chuan dung khi so sanh benchmark MILP/heuristic hoc thuat: "
                  "ty le giai duoc toi uu (gap=0), so ca cham timeout, gap trung binh tren "
                  "cac ca chua toi uu, thoi gian trung binh va **shifted geometric mean** "
                  "thoi gian (chuan hon trung binh so hoc vi it bi lech boi outlier).\n")
    for rs in (rs_a, rs_b):
        lines.append(f"### {rs.name}\n")
        lines.append("| Method | #instance | #toi uu (gap=0) | %toi uu | #timeout | gap TB (%) | time TB (s) | geomean time (s) |")
        lines.append("|---|---|---|---|---|---|---|---|")
        for method in sorted(rs.methods()):
            s = method_stats(rs, method)
            label = METHOD_LABELS.get(method, method)
            lines.append(
                f"| {method} ({label}) | {s['n_instances']} | {s['n_solved_optimal']} | "
                f"{fmt(s['pct_solved_optimal'],1)} | {s['n_timeouts']} | "
                f"{fmt(s['avg_gap_pct'],2)} | {fmt(s['avg_time_s'],1)} | {fmt(s['shifted_geomean_time_s'],1)} |"
            )
        lines.append("")

    # --- Head to head ---
    lines.append("## 3. So sanh truc tiep (head-to-head) tren instance chung\n")
    if not common_methods:
        lines.append("*Chua co phuong phap nao ton tai o ca 2 nguon nen chua the so sanh truc tiep. "
                      "Muc nay se tu sinh ra ngay khi 2 file co it nhat 1 phuong phap trung ten.*\n")
    else:
        lines.append(f"'Thang' duoc tinh theo thu tu uu tien: gap nho hon truoc, roi den time nho hon. "
                      f"'delta = {rs_b.name} - {rs_a.name}' (am nghia la {rs_b.name} tot hon / nhanh hon).\n")
        lines.append(f"| Method | #instance chung | thang {rs_a.name} | thang {rs_b.name} | hoa | delta gap TB (%) | delta time TB (s) |")
        lines.append("|---|---|---|---|---|---|---|")
        for method in common_methods:
            h = head_to_head(rs_a, rs_b, method)
            lines.append(
                f"| {method} | {h['n_common_instances']} | {h[f'wins_{rs_a.name}']} | "
                f"{h[f'wins_{rs_b.name}']} | {h['ties']} | "
                f"{fmt(h['avg_gap_diff_B_minus_A'],3)} | {fmt(h['avg_time_diff_B_minus_A'],2)} |"
            )
        lines.append("")

    lines.append("## 4. Ghi chu phuong phap luan\n")
    lines.append(f"- Gioi han thoi gian coi la 'timeout': {TIME_LIMIT:.0f}s (sua bien `TIME_LIMIT` trong script neu khac).")
    lines.append("- 'Giai toi uu' = gap <= 0 (dung nguong 1e-9 de tranh loi so thuc).")
    lines.append("- Khi 1 instance/method chi ton tai o 1 nguon, no khong duoc tinh vao head-to-head "
                  "nhung van co trong `comparison_details.csv` (cot `present_in`).")
    lines.append("- Cac dong CSV bi trung instance duoc gom bang trung binh cong; neu ban muon chinh sach "
                  "khac (vd. lay lan chay tot nhat), sua ham `parse_csv`.")

    out_path.write_text("\n".join(lines), encoding="utf-8")


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(description="So sanh 2 file ket qua MDSP (rtf/csv, mo rong duoc).")
    ap.add_argument("file_a")
    ap.add_argument("file_b")
    ap.add_argument("--name-a", default=None)
    ap.add_argument("--name-b", default=None)
    ap.add_argument("--outdir", default=".")
    args = ap.parse_args()

    def load(path, name):
        p = Path(path)
        if p.suffix.lower() == ".rtf":
            return parse_rtf(str(p), name or p.stem)
        elif p.suffix.lower() == ".csv":
            return parse_csv(str(p), name or p.stem)
        else:
            raise ValueError(f"Khong ho tro dinh dang file: {p.suffix}")

    rs_a = load(args.file_a, args.name_a)
    rs_b = load(args.file_b, args.name_b)

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    rows = build_details(rs_a, rs_b)
    write_details_csv(rows, outdir / "comparison_details.csv", rs_a, rs_b)
    write_summary_md(rs_a, rs_b, outdir / "comparison_summary.md")

    print(f"Da ghi: {outdir / 'comparison_details.csv'}")
    print(f"Da ghi: {outdir / 'comparison_summary.md'}")


if __name__ == "__main__":
    main()
