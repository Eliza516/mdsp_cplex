#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<long long> D = {1,2,3,4,5};
    int g = 3;
    std::vector<int> Dg;
    for (auto d : D) {
        long long modVal = d % g;
        if (modVal < 0) modVal += g;
        int dg = static_cast<int>(std::min(modVal, g - modVal));
        Dg.push_back(dg);
    }

    std::cout << "Input: D = {1,2,3,4,5}, g = " << g << "\n";
    std::cout << "Transformed D_g: {";
    for (size_t i = 0; i < Dg.size(); ++i) {
        if (i) std::cout << ",";
        std::cout << Dg[i];
    }
    std::cout << "}\n";

    std::vector<int> m(g / 2 + 1, 0);
    for (int v : Dg) if (v >= 0 && v <= g/2) m[v]++;

    std::cout << "Frequencies:\n";
    for (size_t j = 0; j < m.size(); ++j) {
        std::cout << "  m_" << j << " = " << m[j] << "\n";
    }

    return 0;
}
