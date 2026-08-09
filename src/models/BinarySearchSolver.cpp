#include "models/BinarySearchSolver.h"
#include "models/FeasModel.h"
#include <chrono>
#include <iostream>

MDSPSolution BinarySearchSolver::solve(const MDSPInstance& inst, int l, int u, long long B,
                                       double totalTimeLimitSeconds, bool verbose) {
    MDSPSolution bestSol;
    bestSol.bestBound = l;

    int t_LB = l;
    int t_UB = u;

    auto startClock = std::chrono::steady_clock::now();
    double accumulatedCplexTime = 0.0;

    while (t_LB <= t_UB) {
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - startClock).count();
        double remainingTime = totalTimeLimitSeconds - elapsed;
        if (remainingTime <= 0) break;

        int t_MID = (t_LB + t_UB) / 2;

        if (verbose) {
            std::cout << "[BinarySearch] Testing t = " << t_MID << " in range ["
                      << t_LB << ", " << t_UB << "] ... " << std::flush;
        }

        try {
            FeasModel feas(inst, t_MID, B, remainingTime);
            MDSPSolution sol = feas.solve(/*verbose=*/false);
            accumulatedCplexTime += sol.cplexTimeSeconds;

            if (sol.feasible) {
                if (verbose) std::cout << "FEASIBLE\n";
                bestSol = sol;
                bestSol.optimal = (t_LB == t_UB) && !sol.timeLimitReached;
                bestSol.bestBound = t_LB;
                t_UB = t_MID - 1;
            } else {
                if (sol.timeLimitReached) {
                    if (verbose) std::cout << "TIMEOUT (unknown feasibility)\n";
                    break;
                } else {
                    if (verbose) std::cout << "INFEASIBLE\n";
                    t_LB = t_MID + 1;
                    bestSol.bestBound = t_LB;
                }
            }
        } catch (const std::exception& e) {
            if (verbose) std::cout << "ERROR (" << e.what() << ")\n";
            break;
        }
    }

    bestSol.cplexTimeSeconds = accumulatedCplexTime;
    if (bestSol.feasible && bestSol.bestBound == bestSol.objective) {
        bestSol.optimal = true;
    }
    return bestSol;
}
