#include "models/FeasModel.h"
#include <ilcplex/ilocplex.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>

ILOSTLBEGIN

// START: resource-control additions
FeasModel::FeasModel(const MDSPInstance& inst, int t, long long B, double timeLimitSeconds,
                                         int threads, int workMemMB)
        : instance_(inst), t_(t), B_(B), timeLimit_(timeLimitSeconds),
            threads_(threads), workMemMB_(workMemMB) {
// END: resource-control additions
    computeMultiplicities();
}

void FeasModel::computeMultiplicities() {
    std::map<long long, int> counts;
    for (long long d : instance_.D) counts[d]++;

    distinctValues_.clear();
    multiplicities_.clear();
    for (const auto& kv : counts) {
        distinctValues_.push_back(kv.first);
        multiplicities_.push_back(kv.second);
    }
}

MDSPSolution FeasModel::solve(bool verbose) {
    MDSPSolution result;
    IloEnv env;

    try {
        IloModel model(env, "MDSP_P2");

        const int t = t_;
        const long long B = B_;
        const int nd = static_cast<int>(distinctValues_.size());
        const double M = static_cast<double>(B);

        IloNumVarArray p(env, t, 0, B, ILOINT);
        for (int i = 0; i < t; ++i) {
            p[i].setName(("p_" + std::to_string(i + 1)).c_str());
        }

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

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            const long long d = distinctValues_[dIdx];
            for (int i = 0; i < t; ++i) {
                for (int j = i + 1; j < t; ++j) {
                    model.add(p[i] + x[dIdx][i][j] * static_cast<double>(d) <= p[j]);
                    model.add(p[j] <= p[i] + M * (1 - x[dIdx][i][j]) + static_cast<double>(d));
                }
            }
        }

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            IloExpr sum(env);
            for (int i = 0; i < t; ++i)
                for (int j = i + 1; j < t; ++j)
                    sum += x[dIdx][i][j];
            model.add(sum >= multiplicities_[dIdx]);
            sum.end();
        }

        for (int i = 0; i + 1 < t; ++i) {
            model.add(p[i] + 1 <= p[i + 1]);
        }

        model.add(p[0] == 0);

        IloCplex cplex(model);
        // Apply resource-control parameters (threads, work memory)
        cplex.setParam(IloCplex::Param::Threads, threads_);
        cplex.setParam(IloCplex::Param::WorkMem, workMemMB_);
        cplex.setParam(IloCplex::Param::TimeLimit, timeLimit_);
        if (!verbose) cplex.setOut(env.getNullStream());

        auto t0_solve = std::chrono::steady_clock::now();
        bool ok = cplex.solve();
        auto t1_solve = std::chrono::steady_clock::now();
        result.feasible = ok;
        auto status = cplex.getStatus();
        if (!ok && status != IloAlgorithm::Infeasible) {
            result.timeLimitReached = true;
        }
        result.cplexTimeSeconds = std::chrono::duration<double>(t1_solve - t0_solve).count();

        if (ok) {
            result.optimal = true;
            result.objective = static_cast<double>(t);

            std::vector<long long> pts;
            for (int i = 0; i < t; ++i) {
                long long val = static_cast<long long>(std::llround(cplex.getValue(p[i])));
                pts.push_back(val);
            }
            std::sort(pts.begin(), pts.end());
            result.points = std::move(pts);
        }

        cplex.end();
    } catch (IloException& e) {
        env.end();
        throw std::runtime_error(std::string("CPLEX error in FeasModel: ") + e.getMessage());
    } catch (...) {
        env.end();
        throw;
    }

    env.end();
    return result;
}
