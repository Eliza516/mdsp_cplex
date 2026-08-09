#pragma once
#include "core/Types.h"

class CyclicLowerBound {
public:
    static int computeLowerBound(const MDSPInstance& inst);

private:
    static int computeForG(const MDSPInstance& inst, int g);
    static bool checkConstraints(const std::vector<int>& x, int g,
                                 const std::vector<int>& m);
};
