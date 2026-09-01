#!/usr/bin/env python3
"""
So sánh kết quả preprocessing của code với paper (table_4_extracted.csv).

Cách dùng:
    python3 compare_with_paper.py <results.md> [paper.csv]

Output tự động:
    preprocessing_compare.md   — báo cáo đầy đủ dạng markdown
    comparison_diff.csv     — chi tiết từng instance khác nhau dạng CSV
"""

import csv, sys, re, os
from collections import defaultdict
from datetime import datetime

# ──────────────────────────────────────────────────────────────────────────────
# Cấu hình metric
# ──────────────────────────────────────────────────────────────────────────────
METRICS = [
    # (key,    label,         better,   col_in_md)
    ("tLB",  "Trivial LB",  "higher",  2),
    ("tUB",  "Trivial UB",  "lower",   3),
    ("HD",   "Thorn Arcs",  "equal",   4),
    ("nLB",  "Cyclic LB",   "higher",  6),
    ("nUB",  "Arith UB",    "lower",   8),
    ("nDUB", "LNS UB",      "lower",   10),
]
PAPER_CSV  = "table_4_extracted.csv"
PAPER_COLS = {
    "tLB":"Trivial_tLB","tUB":"Trivial_tUB",
    "HD":"HD_HD","nLB":"LB_nLB","nUB":"UB_nUB","nDUB":"DUB_nDUB",
}
OUT_MD  = "compare/results/preprocessing_compare.md"

# ──────────────────────────────────────────────────────────────────────────────
# I/O
# ──────────────────────────────────────────────────────────────────────────────
def load_paper(path):
    data = {}
    with open(path, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            name = row["Instances"].strip()
            data[name] = {k: int(row[v]) for k, v in PAPER_COLS.items()}
    return data

def load_code(path):
    data = {}
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line.startswith("|") or "---|" in line or "Instances" in line:
                continue
            parts = [p.strip() for p in line.strip("|").split("|")]
            if len(parts) < 12:
                continue
            try:
                name = re.sub(r"\.txt$", "", parts[0])
                data[name] = {key: int(parts[col]) for key, _, _, col in METRICS}
            except (ValueError, IndexError):
                continue
    return data

# ──────────────────────────────────────────────────────────────────────────────
# Phân tích
# ──────────────────────────────────────────────────────────────────────────────
def analyze(paper, code):
    better_dir = {m[0]: m[2] for m in METRICS}
    all_names  = sorted(set(paper) | set(code))
    matched, exact = [], []
    only_paper = [n for n in all_names if n not in code]
    only_code  = [n for n in all_names if n not in paper]
    metric_diffs = defaultdict(list)   # key -> [(name,pv,cv,delta,who)]

    for name in all_names:
        if name not in paper or name not in code:
            continue
        matched.append(name)
        p, c = paper[name], code[name]
        any_diff = False
        for key, _, direction, _ in METRICS:
            pv, cv = p[key], c[key]
            if pv == cv:
                continue
            any_diff = True
            delta = cv - pv
            if   direction == "higher": who = "CODE"  if cv > pv else "PAPER"
            elif direction == "lower":  who = "CODE"  if cv < pv else "PAPER"
            else:                       who = "DIFF"
            metric_diffs[key].append((name, pv, cv, delta, who))
        if not any_diff:
            exact.append(name)

    return matched, only_paper, only_code, metric_diffs, exact

# ──────────────────────────────────────────────────────────────────────────────
# Xuất CSV
# ──────────────────────────────────────────────────────────────────────────────
def export_csv(metric_diffs, path):
    rows = []
    for key, label, direction, _ in METRICS:
        for name, pv, cv, delta, who in metric_diffs[key]:
            group = name.split("-")[0]
            rows.append({
                "Instance": name,
                "Group":    group,
                "Metric":   label,
                "Paper":    pv,
                "Code":     cv,
                "Delta":    delta,
                "Better":   who,
                "Direction": direction,
            })
    rows.sort(key=lambda r: (r["Metric"], r["Instance"]))
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["Instance","Group","Metric","Paper","Code","Delta","Better","Direction"])
        writer.writeheader()
        writer.writerows(rows)
    return len(rows)

# ──────────────────────────────────────────────────────────────────────────────
# Xuất Markdown
# ──────────────────────────────────────────────────────────────────────────────
def export_md(paper, code, matched, only_paper, only_code,
              metric_diffs, exact, code_file, paper_file, path):
    lines = []
    ts = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    def h(level, text): lines.append(f"{'#'*level} {text}\n")
    def p(text=""): lines.append(text + "\n")
    def rule(): lines.append("---\n")

    total = len(matched)

    h(1, "Báo cáo So sánh: Code vs Paper")
    p(f"- **Thời gian**: {ts}")
    p(f"- **File code**: `{code_file}`")
    p(f"- **File paper**: `{paper_file}`")
    rule()

    # ── Tổng quan ──
    h(2, "1. Tổng quan")
    p(f"| Mục | Giá trị |")
    p(f"|---|---|")
    p(f"| Instances trong paper | {len(paper)} |")
    p(f"| Instances trong code | {len(code)} |")
    p(f"| Matched (so sánh được) | {total} |")
    if only_code:  p(f"| Chỉ có trong code | {len(only_code)} |")
    if only_paper: p(f"| ⚠️ Thiếu trong code | {len(only_paper)} |")
    p(f"| Khớp hoàn toàn | **{len(exact)} / {total}** ({100*len(exact)//max(total,1)}%) |")
    p()

    # ── Bảng tổng quan metric ──
    h(2, "2. Tổng quan theo Metric")
    p("| Metric | Giống | Khác | Code tốt hơn | Paper tốt hơn | Hướng |")
    p("|---|---|---|---|---|---|")
    for key, label, direction, _ in METRICS:
        diffs   = metric_diffs[key]
        n_diff  = len(diffs)
        n_same  = total - n_diff
        n_code  = sum(1 for *_,who in diffs if who=="CODE")
        n_paper = sum(1 for *_,who in diffs if who=="PAPER")
        status  = "✅" if n_diff==0 else ("⚠️" if n_diff<=5 else "❌")
        note    = "↑ lớn hơn = tốt" if direction=="higher" else ("↓ nhỏ hơn = tốt" if direction=="lower" else "= bằng nhau")
        p(f"| {status} {label} | {n_same} | {n_diff} | {n_code or '-'} | {n_paper or '-'} | {note} |")
    p()

    # ── Kết luận ai tốt hơn ──
    h(2, "3. Kết luận: Ai tốt hơn tổng thể?")
    p("| Metric | Kết quả | Code tốt | Paper tốt |")
    p("|---|---|---|---|")
    for key, label, direction, _ in METRICS:
        diffs   = metric_diffs[key]
        if not diffs or direction=="equal":
            p(f"| {label} | ✅ Giống nhau hoàn toàn | - | - |")
            continue
        n_code  = sum(1 for *_,who in diffs if who=="CODE")
        n_paper = sum(1 for *_,who in diffs if who=="PAPER")
        if   n_code  > n_paper: verdict = "✅ **CODE tốt hơn**"
        elif n_paper > n_code:  verdict = "⚠️ **PAPER tốt hơn**"
        else:                    verdict = "🟰 Ngang nhau"
        p(f"| {label} | {verdict} | {n_code} | {n_paper} |")
    p()

    # ── Theo nhóm ──
    h(2, "4. Thống kê theo Nhóm Instance")
    groups = defaultdict(list)
    for name in matched:
        groups[name.split("-")[0]].append(name)

    for gname in sorted(groups):
        gset  = set(groups[gname])
        total_g = len(gset)
        diff_names = set()
        for key in [m[0] for m in METRICS]:
            diff_names |= {n for n,*_ in metric_diffs[key] if n in gset}
        exact_g = total_g - len(diff_names)

        h(3, f"Nhóm `{gname.upper()}` — {total_g} instances, {exact_g} khớp hoàn toàn ({100*exact_g//max(total_g,1)}%)")
        p("| Metric | Khác | Code tốt | Paper tốt |")
        p("|---|---|---|---|")
        has_any = False
        for key, label, direction, _ in METRICS:
            d = [(n,pv,cv,delta,who) for n,pv,cv,delta,who in metric_diffs[key] if n in gset]
            if not d: continue
            has_any = True
            n_code  = sum(1 for *_,who in d if who=="CODE")
            n_paper = sum(1 for *_,who in d if who=="PAPER")
            verdict = "✅ CODE" if n_code>n_paper else ("⚠️ PAPER" if n_paper>n_code else "🟰 Ngang")
            p(f"| {label} | {len(d)} | {n_code} | {n_paper} | {verdict} |" if direction!="equal"
              else f"| {label} | {len(d)} | - | - |")
        if not has_any:
            p("_Tất cả instances khớp hoàn toàn_")
        p()

    # ── Chi tiết từng metric ──
    h(2, "5. Chi tiết từng Metric (instances khác nhau)")
    for key, label, direction, _ in METRICS:
        diffs = metric_diffs[key]
        if not diffs: continue
        n_code  = sum(1 for *_,who in diffs if who=="CODE")
        n_paper = sum(1 for *_,who in diffs if who=="PAPER")
        h(3, f"{label} — {len(diffs)} instances khác  (Code tốt: {n_code} | Paper tốt: {n_paper})")
        p("| Instance | Nhóm | Paper | Code | Δ | Ai tốt |")
        p("|---|---|---|---|---|---|")
        for name, pv, cv, delta, who in sorted(diffs):
            grp   = name.split("-")[0]
            arrow = f"+{delta}" if delta>0 else str(delta)
            icon  = "✅ CODE" if who=="CODE" else ("⚠️ PAPER" if who=="PAPER" else "≠")
            p(f"| `{name}` | {grp} | {pv} | {cv} | {arrow} | {icon} |")
        p()

    # Ghi file
    with open(path, "w", encoding="utf-8") as f:
        f.write("".join(lines))

# ──────────────────────────────────────────────────────────────────────────────
# In stdout
# ──────────────────────────────────────────────────────────────────────────────
def print_results(paper, code, matched, only_paper, only_code, metric_diffs, exact):
    total = len(matched)
    SEP = "═"*72

    print(f"\n{SEP}")
    print(f"  TỔNG QUAN SO SÁNH: CODE vs PAPER")
    print(f"{SEP}")
    print(f"  Instances paper          : {len(paper)}")
    print(f"  Instances code           : {len(code)}")
    print(f"  Matched                  : {total}")
    if only_paper: print(f"  ⚠️  Thiếu trong code     : {len(only_paper)}")
    if only_code:  print(f"  Extra (chỉ trong code)  : {len(only_code)}")
    print(f"  Khớp hoàn toàn          : {len(exact)} / {total}  ({100*len(exact)//max(total,1)}%)")
    print()
    print(f"  {'Metric':<16} {'Giống':>6} {'Khác':>6} {'Code✅':>8} {'Paper⚠️':>9}  Hướng")
    print(f"  {'─'*16} {'─'*6} {'─'*6} {'─'*8} {'─'*9}  {'─'*20}")
    for key, label, direction, _ in METRICS:
        diffs   = metric_diffs[key]
        n_diff  = len(diffs)
        n_code  = sum(1 for *_,who in diffs if who=="CODE")
        n_paper = sum(1 for *_,who in diffs if who=="PAPER")
        status  = "✅" if n_diff==0 else ("⚠️ " if n_diff<=5 else "❌")
        note    = "↑ lớn hơn=tốt" if direction=="higher" else ("↓ nhỏ hơn=tốt" if direction=="lower" else "= bằng=đúng")
        cs = str(n_code)  if n_code  else "-"
        ps = str(n_paper) if n_paper else "-"
        print(f"  {status} {label:<14} {total-n_diff:>6} {n_diff:>6} {cs:>8} {ps:>9}  {note}")

    print(f"\n{'─'*72}")
    print(f"  KẾT LUẬN TỔNG THỂ")
    print(f"{'─'*72}")
    for key, label, direction, _ in METRICS:
        diffs = metric_diffs[key]
        if not diffs or direction=="equal":
            print(f"  {label:<16}: ✅ Khớp hoàn toàn")
            continue
        n_code  = sum(1 for *_,who in diffs if who=="CODE")
        n_paper = sum(1 for *_,who in diffs if who=="PAPER")
        if   n_code  > n_paper: v = f"✅ CODE tốt hơn  ({n_code} vs {n_paper})"
        elif n_paper > n_code:  v = f"⚠️  PAPER tốt hơn ({n_paper} vs {n_code})"
        else:                    v = f"🟰 Ngang nhau    ({n_code} vs {n_paper})"
        print(f"  {label:<16}: {v}")

    print(f"\n{'─'*72}")
    print(f"  THEO NHÓM INSTANCE")
    print(f"{'─'*72}")
    groups = defaultdict(list)
    for name in matched: groups[name.split("-")[0]].append(name)
    for gname in sorted(groups):
        gset = set(groups[gname])
        diff_names = set()
        for key in [m[0] for m in METRICS]:
            diff_names |= {n for n,*_ in metric_diffs[key] if n in gset}
        exact_g = len(gset) - len(diff_names)
        print(f"\n  ┌─ [{gname.upper()}]  {len(gset)} instances  |  {exact_g} khớp ({100*exact_g//max(len(gset),1)}%)")
        for key, label, direction, _ in METRICS:
            d = [(n,pv,cv,delta,who) for n,pv,cv,delta,who in metric_diffs[key] if n in gset]
            if not d: continue
            nc = sum(1 for *_,who in d if who=="CODE")
            np = sum(1 for *_,who in d if who=="PAPER")
            bar = f"CODE {nc} vs PAPER {np}" + (" → CODE✅" if nc>np else (" → PAPER⚠️" if np>nc else " → 🟰"))
            print(f"  │  {label:<14}: {len(d):3d} khác  [{bar}]")
        print(f"  └{'─'*60}")

# ──────────────────────────────────────────────────────────────────────────────
# Main
# ──────────────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Cách dùng: python3 {sys.argv[0]} <results.md> [paper.csv]")
        sys.exit(1)

    code_file  = sys.argv[1]
    paper_file = sys.argv[2] if len(sys.argv) > 2 else PAPER_CSV

    for f in [code_file, paper_file]:
        if not os.path.exists(f):
            print(f"❌ Không tìm thấy: {f}"); sys.exit(1)

    paper = load_paper(paper_file)
    code  = load_code(code_file)
    matched, only_paper, only_code, metric_diffs, exact = analyze(paper, code)

    # In ra màn hình
    print_results(paper, code, matched, only_paper, only_code, metric_diffs, exact)

    # Xuất file
    export_md(paper, code, matched, only_paper, only_code,
              metric_diffs, exact, code_file, paper_file, OUT_MD)

    print(f"\n{'═'*72}")
    print(f"  📁 FILE OUTPUT")
    print(f"{'═'*72}")
    print(f"  📝 MD   → {OUT_MD}")
    print()
