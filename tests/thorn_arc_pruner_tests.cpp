#include "bounds/ThornArcPruner.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static void expectVectorEqual(const std::vector<long long>& actual,
                             const std::vector<long long>& expected,
                             const std::string& label) {
    if (actual != expected) {
        std::cerr << "FAIL: " << label << "\n"
                  << "  expected: ";
        for (long long v : expected) std::cerr << v << ' ';
        std::cerr << "\n  actual:   ";
        for (long long v : actual) std::cerr << v << ' ';
        std::cerr << "\n";
        std::exit(1);
    }
}

static void expectBool(bool actual, bool expected, const std::string& label) {
    if (actual != expected) {
        std::cerr << "FAIL: " << label << "\n"
                  << "  expected: " << expected << "\n"
                  << "  actual:   " << actual << "\n";
        std::exit(1);
    }
}

static void checkCase(const std::vector<long long>& input,
                      bool expectedApplied,
                      const std::vector<long long>& expectedPruned,
                      const std::vector<long long>& expectedThorns,
                      const std::string& caseName) {
    MDSPInstance inst;
    inst.D = input;

    PruningResult res = ThornArcPruner::prune(inst);

    expectBool(res.applied, expectedApplied, caseName + " :: applied");
    expectVectorEqual(res.prunedInstance.D, expectedPruned, caseName + " :: prunedInstance.D");
    expectVectorEqual(res.thornArcs, expectedThorns, caseName + " :: thornArcs");
}

int main() {
    // Standalone test-only harness for ThornArcPruner.
    // This file is intentionally isolated and does not modify the solver logic.
    // It compiles only the pruner implementation and the minimal supporting headers.

    checkCase({1, 2, 3, 4, 5, 6, 7, 100},
              true,
              {1, 2, 3, 4, 5, 6, 7},
              {100},
              "case_1");

    checkCase({1, 2, 3, 4, 10},
              false,
              {1, 2, 3, 4, 10},
              {},
              "case_2");

    checkCase({1, 2, 3, 4, 100, 200, 300},
              false,
              {1, 2, 3, 4, 100, 200, 300},
              {},
              "case_3");

    std::cout << "ThornArcPruner tests passed (3 cases).\n";
    return 0;
}
