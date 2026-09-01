#!/bin/bash
set -e

# Use flock to ensure only one instance of this script runs at a time
exec 9>/tmp/mdsp_experiment.lock
if ! flock -n 9; then
    echo "Another instance of run_experiments.sh is already running."
    echo "Please stop it before starting a new one."
    exit 1
fi

# Số luồng (thread) cho mỗi instance (CPLEX work threads)
THREADS=1
# Số lượng instances chạy song song
CONCURRENCY=5
# File output
OUT_CSV="results.csv"

# Các thư mục chứa instances theo đúng thứ tự
DIRS=("mdsp_instances/full" "mdsp_instances/miss" "mdsp_instances/joint" "mdsp_instances/drand" "mdsp_instances/luci")

echo "======================================================"
echo "Starting experiments with:"
echo " - Threads per instance: $THREADS"
echo " - Parallel instances  : $CONCURRENCY"
echo " - Output CSV          : $OUT_CSV"
echo "======================================================"

for dir in "${DIRS[@]}"; do
    if [ -d "$dir" ] && [ "$(ls -A $dir 2>/dev/null)" ]; then
        echo ">>> Running instances in $dir..."
        # --quiet is used to prevent too much console output when running concurrently
        bin/mdsp_solver --batch "$dir" --out "$OUT_CSV" --threads "$THREADS" --concurrency "$CONCURRENCY" --time 3600 --quiet
    else
        echo ">>> Skipping $dir (directory empty or does not exist)"
    fi
done

echo "======================================================"
echo "All experiments completed successfully."
echo "Results are saved in $OUT_CSV"
echo "======================================================"

