// =============================================================================
// MaxModel — Paper's MAX variant (binary search + maximum coverage)
//
// Given t points, this model MAXIMIZES the number of distances covered.
// If max coverage == |D|, then t is feasible for MDSP.
// Uses the same assignment-based variables as IP/FEAS models:
//   p_i : point positions
//   x^d_{ij} : binary indicator for distance assignment
//
// Objective: maximize Σ_d Σ_{i<j} x^d_{ij}
// (equivalent to maximizing the total number of distance assignments)
// =============================================================================

#include "models/MaxModel.h"
#include <ilcplex/ilocplex.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>

ILOSTLBEGIN

MDSPMaxModel::MDSPMaxModel(const MDSPInstance& inst, int l, int u, long long B, double timeLimitSeconds,
                                         int threads, int workMemMB)
        : instance_(inst), l_(l), u_(u), B_(B), timeLimit_(timeLimitSeconds),
            threads_(threads), workMemMB_(workMemMB) {
    computeMultiplicities();
}

void MDSPMaxModel::computeMultiplicities() {
    std::map<long long, int> counts;
    for (long long d : instance_.D) counts[d]++;

    distinctValues_.clear();
    multiplicities_.clear();
    for (const auto& kv : counts) {
        distinctValues_.push_back(kv.first);
        multiplicities_.push_back(kv.second);
    }
}

// solve() uses u_ as the target size (standalone optimization mode)
MDSPSolution MDSPMaxModel::solve(bool verbose) {
    return solveForT(u_, timeLimit_, verbose);
}

// solveForT() builds a max-coverage MIP for a specific target t
MDSPSolution MDSPMaxModel::solveForT(int t, double timeLimitSeconds, bool verbose) {
    MDSPSolution result;
    IloEnv env;

    try {
        IloModel model(env, "MDSP_MAX");

        const long long B = B_;
        const int nd = static_cast<int>(distinctValues_.size());
        const double M = static_cast<double>(B);

        // Point variables: p_i ∈ [0, B], strictly increasing, p_0 = 0
        IloNumVarArray p(env, t, 0, B, ILOINT);
        for (int i = 0; i < t; ++i) {
            p[i].setName(("p_" + std::to_string(i + 1)).c_str());
        }

        // Distance assignment variables: x^d_{ij} ∈ {0, 1}
        std::vector<std::vector<std::vector<IloNumVar>>> x(
            nd, std::vector<std::vector<IloNumVar>>(t, std::vector<IloNumVar>(t)));

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            for (int i = 0; i < t; ++i) {
                for (int j = i + 1; j < t; ++j) {
                    std::string nm = "x_" + std::to_string(distinctValues_[dIdx]) +
                                      "_" + std::to_string(i + 1) + "_" + std::to_string(j + 1);
                    x[dIdx][i][j] = IloBoolVar(env, nm.c_str());
                }
            }
        }

        // Constraints (2)-(3): linking x^d_{ij} to point positions
        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            const long long d = distinctValues_[dIdx];
            for (int i = 0; i < t; ++i) {
                for (int j = i + 1; j < t; ++j) {
                    model.add(p[i] + x[dIdx][i][j] * static_cast<double>(d) <= p[j]);
                    model.add(p[j] <= p[i] + M * (1 - x[dIdx][i][j]) + static_cast<double>(d));
                }
            }
        }

        // Constraint: each distance d can be covered at most mult(d) times
        // (soft: we maximize coverage, so ≤ mult(d) is the cap)
        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            IloExpr sum(env);
            for (int i = 0; i < t; ++i)
                for (int j = i + 1; j < t; ++j)
                    sum += x[dIdx][i][j];
            model.add(sum <= multiplicities_[dIdx]);
            sum.end();
        }

        // Constraint: each point is left/right endpoint at most once per distance
        for (int i = 0; i < t; ++i) {
            for (int dIdx = 0; dIdx < nd; ++dIdx) {
                IloExpr sumAll(env);
                for (int j = 0; j < t; ++j) {
                    if (j > i)      sumAll += x[dIdx][i][j];
                    else if (j < i) sumAll += x[dIdx][j][i];
                }
                model.add(sumAll <= 1);
                sumAll.end();
            }
        }

        // Ordering constraints
        for (int i = 0; i + 1 < t; ++i) {
            model.add(p[i] + 1 <= p[i + 1]);
        }

        model.add(p[0] == 0);

        // Objective: MAXIMIZE total distances covered
        IloExpr obj(env);
        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            for (int i = 0; i < t; ++i)
                for (int j = i + 1; j < t; ++j)
                    obj += x[dIdx][i][j];
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::Threads, threads_);
        cplex.setParam(IloCplex::Param::WorkMem, workMemMB_);
        cplex.setParam(IloCplex::Param::TimeLimit, timeLimitSeconds);
        if (!verbose) cplex.setOut(env.getNullStream());

        auto t0 = std::chrono::steady_clock::now();
        bool ok = cplex.solve();
        auto t1 = std::chrono::steady_clock::now();

        result.feasible = ok;
        result.cplexTimeSeconds = std::chrono::duration<double>(t1 - t0).count();

        try {
            result.bestBound = cplex.getBestObjValue();
        } catch (...) {
            result.bestBound = -1.0;
        }

        if (ok) {
            double maxCoverage = cplex.getObjValue();
            int totalDistances = static_cast<int>(instance_.D.size());

            // If max coverage == |D|, all distances are covered → feasible for this t
            if (static_cast<int>(maxCoverage + 0.5) >= totalDistances) {
                result.optimal = true;
                result.objective = static_cast<double>(t);

                std::vector<long long> pts;
                for (int i = 0; i < t; ++i) {
                    long long val = static_cast<long long>(std::llround(cplex.getValue(p[i])));
                    pts.push_back(val);
                }
                std::sort(pts.begin(), pts.end());
                result.points = std::move(pts);
            } else {
                // Not all distances covered with t points → infeasible for this t
                result.feasible = false;
                result.objective = maxCoverage;
            }
        }

        auto status = cplex.getStatus();
        if (!ok && status != IloAlgorithm::Infeasible) {
            result.timeLimitReached = true;
        }

        cplex.end();
    } catch (IloException& e) {
        env.end();
        throw std::runtime_error(std::string("CPLEX/Concert error in MaxModel: ") + e.getMessage());
    } catch (...) {
        env.end();
        throw;
    }

    env.end();
    return result;
}
