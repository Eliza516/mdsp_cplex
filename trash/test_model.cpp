#include "core/Types.h"
#include "models/Model.h"
#include <iostream>

int main() {
    MDSPInstance inst;
    inst.D = {5, 5};
    MDSPModel model(inst, 3, 3, 10, 10.0, 1, 1024);
    MDSPSolution sol = model.solve(true);
    std::cout << "Feasible: " << sol.feasible << std::endl;
    if (sol.feasible) {
        for (long long p : sol.points) std::cout << p << " ";
        std::cout << std::endl;
    }
    return 0;
}
