import os
import random
import itertools

# --- Configuration & Setup ---
OUTPUT_DIR = "mdsp_instances"

# Class constraints
N_VALUES = [5, 6, 7, 8, 9, 10]
M_VALUES = [15, 30]
MISS_R_MAP = {5: 2, 6: 3, 7: 3, 8: 4, 9: 4, 10: 5}

DRAND_K = [5, 7, 10, 15, 20, 25, 30, 35, 40, 45, 50, 75, 100, 112]
DRAND_M = [75, 110, 200]

LUCI_BASE = [9, 30, 100, 170, 293, 302, 393, 402, 462, 562, 632, 732, 
             855, 864, 945, 954, 975, 984, 1025, 1034, 1247, 1277, 
             1347, 1377, 1809, 1839, 1979, 2009]

def save_instance(filename, D):
    """Saves the distance multiset to a file following the paper's format."""
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    filepath = os.path.join(OUTPUT_DIR, filename)
    with open(filepath, 'w') as f:
        f.write(f"{len(D)}\n")
        f.write(" ".join(map(str, sorted(D))) + "\n")

def generate_full_set(n, M):
    """
    Generates a full instance:
    1. Sample n distinct integers uniformly from [0, M] without replacement.
    2. Compute all n*(n-1)/2 pairwise absolute differences.
    """
    points = random.sample(range(0, M + 1), n)
    distances = []
    for i in range(n):
        for j in range(i + 1, n):
            distances.append(abs(points[i] - points[j]))
    return sorted(distances)

def remove_random_elements(D, R):
    """Removes R elements by randomly picking indices."""
    D_copy = D.copy()
    for _ in range(R):
        idx = random.randrange(len(D_copy))
        D_copy.pop(idx)
    return D_copy

def main():
    random.seed(42) # For reproducibility 
    
    total_generated = 0

    # ---------------------------------------------------------
    # Class 1: full (10 instances per n, M)
    # ---------------------------------------------------------
    for n in N_VALUES:
        for M in M_VALUES:
            for i in range(1, 11):
                D = generate_full_set(n, M)
                save_instance(f"full-{n}-{M}-{i}.txt", D)
                total_generated += 1

    # ---------------------------------------------------------
    # Class 2: miss (5 instances per n, M, R)
    # ---------------------------------------------------------
    for n in N_VALUES:
        for M in M_VALUES:
            R = MISS_R_MAP[n]
            for i in range(1, 6):
                D_full = generate_full_set(n, M)
                D_miss = remove_random_elements(D_full, R)
                save_instance(f"miss-{n}-{M}-{R}-{i}.txt", D_miss)
                total_generated += 1

    # ---------------------------------------------------------
    # Class 3: joint (3 instances per combination)
    # ---------------------------------------------------------
    # Combining each (n1, M) with (5, M) to create the combinations 
    for n1 in N_VALUES:
        for M in M_VALUES:
            n2 = 5
            for i in range(1, 4):
                D1 = generate_full_set(n1, M)
                D2 = generate_full_set(n2, M)
                # Concatenate multisets, preserving duplicates
                D_joint = D1 + D2 
                save_instance(f"joint-{n1}-{M}-{n2}-{M}-{i}.txt", D_joint)
                total_generated += 1

    # ---------------------------------------------------------
    # Class 4: drand (3 instances per k, M)
    # ---------------------------------------------------------
    for k in DRAND_K:
        for M in DRAND_M:
            for i in range(1, 4):
                # Sample k integers with replacement from [1, M]
                D_rand = random.choices(range(1, M + 1), k=k)
                save_instance(f"drand-{k}-{M}-{i}.txt", D_rand)
                total_generated += 1

    # ---------------------------------------------------------
    # Class 5: luci (Base + 5 instances per R for R in 1..14)
    # ---------------------------------------------------------
    # The full base instance
    save_instance("luci-base.txt", LUCI_BASE)
    total_generated += 1
    
    # The missing instances
    for R in range(1, 15):
        for i in range(0, 5):
            D_luci = remove_random_elements(LUCI_BASE, R)
            save_instance(f"luci-{R}-{i}.txt", D_luci)
            total_generated += 1

    print(f"Dataset generation complete!")
    print(f"Total instances generated: {total_generated} (Target: ~534)")

if __name__ == "__main__":
    main()