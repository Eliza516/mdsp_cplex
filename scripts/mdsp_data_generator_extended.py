import os
import random

OUTPUT_DIR = "mdsp_instances"

# --- Expanded Configurations based on Paper Results (Tables 2-9) ---
FULL_N = range(5, 17)    # 5 to 16
MISS_N = range(5, 18)    # 5 to 17
JOINT_N1 = range(5, 17)  # 5 to 16
M_VALUES = [15, 30]

# drand k values derived from Tables 2 & 3 (including 12)
DRAND_K = [5, 7, 10, 12, 15, 20, 25, 30, 35, 40, 45, 50, 75, 100, 112]
DRAND_M = [75, 110, 200]

# Luciferase base biological sequence
LUCI_BASE = [9, 30, 100, 170, 293, 302, 393, 402, 462, 562, 632, 732, 
             855, 864, 945, 954, 975, 984, 1025, 1034, 1247, 1277, 
             1347, 1377, 1809, 1839, 1979, 2009]
LUCI_R_VALUES = range(1, 16) # 1 to 15 based on Table 9

def save_instance(filename, D):
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    filepath = os.path.join(OUTPUT_DIR, filename)
    with open(filepath, 'w') as f:
        f.write(f"{len(D)}\n")
        f.write(" ".join(map(str, sorted(D))) + "\n")

def generate_full_set(n, M):
    points = random.sample(range(0, M + 1), n)
    distances = []
    for i in range(n):
        for j in range(i + 1, n):
            distances.append(abs(points[i] - points[j]))
    return sorted(distances)

def remove_random_elements(D, R):
    D_copy = D.copy()
    for _ in range(R):
        if D_copy:
            idx = random.randrange(len(D_copy))
            D_copy.pop(idx)
    return D_copy

def main():
    random.seed(42)
    total_generated = 0
    
    print("Generating FULL instances...")
    for n in FULL_N:
        for M in M_VALUES:
            if n > M + 1:  # not enough distinct integers in [0, M]
                continue
            # 5 instances per configuration (to match exactly with Table 4 limits)
            for i in range(1, 6):
                D = generate_full_set(n, M)
                save_instance(f"full-{n}-{M}-{i}.txt", D)
                total_generated += 1

    print("Generating MISS instances...")
    for n in MISS_N:
        for M in M_VALUES:
            if n > M + 1:  # not enough distinct integers in [0, M]
                continue
            # R rule derived from Tables: R = floor((n-1)/2)
            R = (n - 1) // 2
            for i in range(1, 6):
                D_full = generate_full_set(n, M)
                D_miss = remove_random_elements(D_full, R)
                save_instance(f"miss-{n}-{M}-{R}-{i}.txt", D_miss)
                total_generated += 1

    print("Generating JOINT instances...")
    for n1 in JOINT_N1:
        for M in M_VALUES:
            n2 = 5
            if n1 > M + 1 or n2 > M + 1:  # not enough distinct integers in [0, M]
                continue
            for i in range(1, 4):
                D1 = generate_full_set(n1, M)
                D2 = generate_full_set(n2, M)
                D_joint = D1 + D2
                save_instance(f"joint-{n1}-{M}-{n2}-{M}-{i}.txt", D_joint)
                total_generated += 1

    print("Generating DRAND instances...")
    for k in DRAND_K:
        for M in DRAND_M:
            for i in range(1, 4):
                D_rand = random.choices(range(1, M + 1), k=k)
                save_instance(f"drand-{k}-{M}-{i}.txt", D_rand)
                total_generated += 1

    print("Generating LUCI instances...")
    save_instance("luciferase-full.txt", LUCI_BASE)
    total_generated += 1
    
    for R in LUCI_R_VALUES:
        for i in range(0, 5):
            D_luci = remove_random_elements(LUCI_BASE, R)
            save_instance(f"luciferase-removed-{R}-{i}.txt", D_luci)
            total_generated += 1

    print(f"Dataset generation complete! Total instances: {total_generated}")

if __name__ == "__main__":
    main()
