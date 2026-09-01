# MDSP Solver (C++ / CPLEX)

A clean C++ implementation with IBM ILOG CPLEX (Concert Technology) of the algorithms and Mixed Integer Linear Programming formulations described in:

> Nguyen, M.H., Nguyen, N.A., Pham, H., Do, D.D., Nguyen, D., Ha, M.H.,
> *"Improved Results for the Minimum Distance Superset Problem"*.

Given a multiset of positive integers `D`, the program finds the smallest point set `P` on the real line such that `D` is a sub-multiset of `Delta(P) = { pi - pj | pi, pj in P, pi > pj }`.

## Project Structure

```text
mdsp_cplex/
├── include/
│   ├── core/                  # Data types & File I/O
│   │   ├── Types.h            # MDSPInstance, MDSPBounds, MDSPSolution
│   │   └── Reader.h           # Instance file I/O
│   ├── bounds/                # Pre-processing & Bound Calculators
│   │   ├── Bounds.h           # Trivial bounds & Theorem 3 value bound B
│   │   ├── ThornArcPruner.h   # Section 4.1 DP Thorn-arc pruning
│   │   ├── CyclicLowerBound.h # Section 4.2 Z/gZ ring lower bound
│   │   └── LNSUpperBound.h    # Section 4.4 Algorithm 4 LNS heuristic
│   ├── models/                # Optimization Models & Solvers
│   │   ├── Model.h            # Section 3 IP formulation (P1)
│   │   ├── FeasModel.h        # Section 3 Feasibility formulation (P2)
│   │   └── BinarySearchSolver.h # Algorithm 1 FEAS binary search solver
│   └── runner/                # Experiment Runner & CSV Exporter
│       ├── CsvExporter.h      # Streaming CSV writer
│       └── BatchRunner.h      # Directory batch runner
├── src/
│   ├── core/
│   │   └── Reader.cpp
│   ├── bounds/
│   │   ├── Bounds.cpp
│   │   ├── ThornArcPruner.cpp
│   │   ├── CyclicLowerBound.cpp
│   │   └── LNSUpperBound.cpp
│   ├── models/
│   │   ├── Model.cpp
│   │   ├── FeasModel.cpp
│   │   └── BinarySearchSolver.cpp
│   ├── runner/
│   │   ├── CsvExporter.cpp
│   │   └── BatchRunner.cpp
│   └── main.cpp               # Main CLI entry point
├── data/
│   └── full-5-15-1.txt
├── Makefile
└── README.md
```

## Building

```bash
make
```

## Running

Single instance:
```bash
./bin/mdsp_solver data/full-5-15-1.txt 3600 --algo p1
# or binary search algorithm:
./bin/mdsp_solver data/full-5-15-1.txt 3600 --algo feas
```

Resource-control flags (to limit CPU/memory usage and avoid freezing):

```bash
# Limit to 1 thread and 1 GB CPLEX work memory (safe default on laptops)
./bin/mdsp_solver data/full-5-15-1.txt 3600 --algo p1 --threads 1 --workmem 1024

# Batch mode example with limits
./bin/mdsp_solver --batch data/ --out results.csv --time 1800 --algo p1 --threads 2 --workmem 2048 --concurrency 1
```

Batch mode (all files in a directory):
```bash
./bin/mdsp_solver --batch data/ --out results.csv --time 3600 --algo p1
```
