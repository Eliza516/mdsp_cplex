#include "models/Model.h"
#include <ilcplex/ilocplex.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>

ILOSTLBEGIN

MDSPModel::MDSPModel(const MDSPInstance& inst, int l, int u, long long B, double timeLimitSeconds)
    : instance_(inst), l_(l), u_(u), B_(B), timeLimit_(timeLimitSeconds) {
    computeMultiplicities();
}

void MDSPModel::computeMultiplicities() {
    std::map<long long, int> counts;
    for (long long d : instance_.D) counts[d]++;

    distinctValues_.clear();
    multiplicities_.clear();
    distinctValues_.reserve(counts.size());
    multiplicities_.reserve(counts.size());
    for (const auto& kv : counts) {
        distinctValues_.push_back(kv.first);
        multiplicities_.push_back(kv.second);
    }
}

MDSPSolution MDSPModel::solve(bool verbose) {
    MDSPSolution result;

    IloEnv env;
    try {
        IloModel model(env, "MDSP_P1");

        const int u  = u_;
        const long long B = B_;
        const int nd = static_cast<int>(distinctValues_.size());
        const double M = static_cast<double>(B);

        IloNumVarArray p(env, u, 0, B, ILOINT);
        for (int i = 0; i < u; ++i) {
            p[i].setName(("p_" + std::to_string(i + 1)).c_str());
        }

        IloNumVarArray z(env, u, 0, 1, ILOBOOL);
        for (int i = 0; i < u; ++i) {
            z[i].setName(("z_" + std::to_string(i + 1)).c_str());
        }

        std::vector<std::vector<std::vector<IloNumVar>>> x(
            nd, std::vector<std::vector<IloNumVar>>(u, std::vector<IloNumVar>(u)));

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            for (int i = 0; i < u; ++i) {
                for (int j = i + 1; j < u; ++j) {
                    std::string nm = "x_" + std::to_string(distinctValues_[dIdx]) +
                                      "_" + std::to_string(i + 1) + "_" + std::to_string(j + 1);
                    x[dIdx][i][j] = IloBoolVar(env, nm.c_str());
                }
            }
        }

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            const long long d = distinctValues_[dIdx];
            for (int i = 0; i < u; ++i) {
                for (int j = i + 1; j < u; ++j) {
                    model.add(p[i] + x[dIdx][i][j] * static_cast<double>(d) <= p[j]);
                    model.add(p[j] <= p[i] + M * (1 - x[dIdx][i][j]) + static_cast<double>(d));
                }
            }
        }

        for (int dIdx = 0; dIdx < nd; ++dIdx) {
            IloExpr sum(env);
            for (int i = 0; i < u; ++i)
                for (int j = i + 1; j < u; ++j)
                    sum += x[dIdx][i][j];
            model.add(sum >= multiplicities_[dIdx]);
            sum.end();
        }

        for (int i = 0; i < u; ++i) {
            for (int dIdx = 0; dIdx < nd; ++dIdx) {
                IloExpr sum(env);
                for (int j = 0; j < u; ++j) {
                    if (j > i)      sum += x[dIdx][i][j];
                    else if (j < i) sum += x[dIdx][j][i];
                }
                model.add(sum <= z[i]);
                sum.end();
            }
        }

        for (int i = 0; i + 1 < u; ++i) {
            model.add(p[i] + 1 <= p[i + 1]);
        }

        model.add(p[0] == 0);
        model.add(z[0] == 1);

        for (int i = 0; i + 1 < u; ++i) {
            model.add(z[i + 1] <= z[i]);
        }

        for (int i = 0; i < l_ && i < u; ++i) {
            model.add(z[i] == 1);
        }

        IloExpr obj(env);
        for (int i = 0; i < u; ++i) obj += z[i];
        model.add(IloMinimize(env, obj));
        obj.end();

        IloCplex cplex(model);
        cplex.setParam(IloCplex::Param::TimeLimit, timeLimit_);
        if (!verbose) cplex.setOut(env.getNullStream());

        auto t0_solve = std::chrono::steady_clock::now();
        bool ok = cplex.solve();
        auto t1_solve = std::chrono::steady_clock::now();
        result.feasible = ok;
        result.cplexTimeSeconds = std::chrono::duration<double>(t1_solve - t0_solve).count();

        try {
            result.bestBound = cplex.getBestObjValue();
        } catch (IloException&) {
            result.bestBound = -1.0;
        }

        if (ok) {
            result.objective = cplex.getObjValue();
            result.optimal = (cplex.getCplexStatus() == IloCplex::Optimal);

            std::vector<long long> pts;
            for (int i = 0; i < u; ++i) {
                if (cplex.getValue(z[i]) > 0.5) {
                    long long val = static_cast<long long>(std::llround(cplex.getValue(p[i])));
                    pts.push_back(val);
                }
            }
            std::sort(pts.begin(), pts.end());
            result.points = std::move(pts);
        }

        cplex.end();
    } catch (IloException& e) {
        std::string msg = std::string("CPLEX/Concert error: ") + e.getMessage();
        env.end();
        throw std::runtime_error(msg);
    } catch (...) {
        env.end();
        throw;
    }

    env.end();
    return result;
}
