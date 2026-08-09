# AI Prompt: Generate MDSP Benchmark Data

---

## PROMPT (copy everything below this line)

---

I am working on the **Minimum Distance Superset Problem (MDSP)**. I need you to write a Python script that generates benchmark datasets **exactly** as described in the following paper:

> *"Improved Results for the Minimum Distance Superset Problem"*
> by M.H. Nguyen, N.A. Nguyen, H. Pham, D.D. Do, D. Nguyen, M.H. Ha.

---

## Problem Background

Given a multiset of positive integers `D`, find the smallest point set `P` on the real line such that `D` is a sub-multiset of all pairwise absolute differences of `P`.

---

## Dataset Description

The paper uses **5 classes of instances**. Each instance is saved as a `.txt` file with this exact format:

```
line 1: k              (integer — the number of distances in D)
line 2: d1 d2 ... dk   (k positive integers, space-separated, any order)
```

---

## Class 1: `full`

**What it is:** The "clean" case — D contains all pairwise distances of n random points.

**How to generate:**
1. Choose `n` (number of points) and `M` (max value of integer interval).
2. Sample **n distinct integers** uniformly at random from `[0, M]` **without replacement**.
3. Compute **all** `n*(n-1)/2` pairwise absolute differences → this is `D`.
4. D is a **multiset**: keep duplicate values if two pairs share the same distance.

**Parameters:**
- `n` ∈ {5, 6, 7, 8, 9, 10}
- `M` ∈ {15, 30}
- Generate **10 independent instances** per `(n, M)` pair.

**Filename:** `full-{n}-{M}-{id}.txt` where `id` goes from 1 to 10.

**Example:** `full-5-15-1.txt` could contain:
```
10
4 6 10 11 15 17 21 26 32 36
```

---

## Class 2: `miss`

**What it is:** A `full` instance with some distances randomly removed — simulates missing measurements.

**How to generate:**
1. Generate the full distance list exactly as in Class 1 (same procedure, different random seed).
2. Randomly remove exactly **R elements** from the list, one at a time. Each removal picks a **uniformly random index** from the current list (so each occurrence of a duplicate is treated as a separate removable element).
3. The remaining distances form D.

**Parameters:**
- Same `n` and `M` as `full`.
- `R` (number of removed distances):
  - `n=5` → `R=2`
  - `n=6` → `R=3`
  - `n=7` → `R=3`
  - `n=8` → `R=4`
  - `n=9` → `R=4`
  - `n=10` → `R=5`
- Generate **5 independent instances** per `(n, M, R)` combination.

**Filename:** `miss-{n}-{M}-{R}-{id}.txt` where `id` goes from 1 to 5.

---

## Class 3: `joint`

**What it is:** Two independent `full` instances merged — creates a larger, harder multiset.

**How to generate:**
1. Generate two independent `full` distance lists `D1` and `D2` (each using the `full` procedure above, with their own random seeds).
2. Concatenate: `D = D1 + D2` (preserve all duplicates from both lists — true multiset union).

**Parameters:**
- Pairs: `(n1=5, M1=15)` + `(n2=5, M2=15)`, and `(n1=10, M1=30)` + `(n2=5, M2=30)`
- Generate **3 independent instances** per parameter pair.

**Filename:** `joint-{n1}-{M1}-{n2}-{M2}-{id}.txt` where `id` goes from 1 to 3.

---

## Class 4: `drand` (random distances)

**What it is:** The hardest class — distances are NOT derived from any point set. They are drawn independently at random.

**How to generate:**
1. Choose `k` (number of distances) and `M` (upper bound).
2. Draw **k integers independently and uniformly at random** from `[1, M]` **with replacement** (duplicates are allowed and expected).
3. These k values directly form D.

**Parameters:**
- `k` ∈ {5, 7, 10, 15, 20, 25, 30, 35, 40, 45, 50, 75, 100, 112}
- `M` ∈ {75, 110, 200}
- Generate **3 independent instances** per `(k, M)` pair.

**Filename:** `drand-{k}-{M}-{id}.txt` where `id` goes from 1 to 3.

---

## Class 5: `luci` (Luciferase — real biological data)

**What it is:** Simulates partial digestion of the Luciferase gene by restriction enzyme **TaqI** (cuts at `TCGA` pattern), as described by Abbas et al. (2016).

**The fixed base distance multiset (28 values — hardcoded, NOT random):**
```python
BASE_D = [9, 30, 100, 170, 293, 302, 393, 402, 462, 562, 632, 732,
          855, 864, 945, 954, 975, 984, 1025, 1034, 1247, 1277,
          1347, 1377, 1809, 1839, 1979, 2009]
```

**How to generate:**
1. Save the base multiset itself as the full Luciferase instance → `luci-0-0.txt`.
2. For each `R` from 1 to 14:
   a. Make a fresh copy of `BASE_D` as a list.
   b. Remove exactly `R` elements by picking a **uniformly random index** each time (treat each element occurrence as separate).
   c. Save the remaining elements as an instance.
   d. Repeat steps a–c **5 times** independently (ids 0 to 4).

**Filename:** `luci-{R}-{i}.txt` where `R` is 1..14 and `i` is 0..4.
**Total:** 1 base + 14×5 = **76 Luciferase instances**.

---

## Critical Rules (DO NOT violate these)

1. **D is always a multiset.** Duplicate distance values must be preserved — never deduplicate D.
2. **For `full`, `miss`, `joint`:** Points are sampled **without replacement** (all points are distinct integers).
3. **For `drand`:** Distances are drawn **with replacement** (duplicates in D are normal and expected).
4. **For `luci`:** The 28 base values are **fixed constants** from biological data. Do not randomize them.
5. **Removal in `miss` and `luci`:** Treat the distance list as an ordered list, pick a **random index** (not a random value) to remove, so each occurrence of a duplicate is independently removable.
6. **All distances must be strictly positive integers** (≥ 1). Since points are distinct, pairwise differences are guaranteed positive.
7. **Output directory:** Save all files into a folder called `data/`, creating it if it doesn't exist.

---

## What the script must do

- Implement each of the 5 generator functions separately: `generate_full`, `generate_miss`, `generate_joint`, `generate_drand`, `generate_luciferase`.
- Each function must: generate D according to the rules, save it to the correct filename in `data/`, and return the filename and D.
- In `main`, call each function with all the parameter combinations listed above to generate the complete benchmark set.
- Accept an optional command-line argument `--seed INT` for reproducibility (set `random.seed(seed)` at the start of main).
- Print a summary at the end: how many files were created per class.

---

## Language and libraries

- Python 3
- Use only the standard library (`random`, `os`, `argparse`).
- No external dependencies.
