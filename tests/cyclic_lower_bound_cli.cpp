#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

struct MDSPInstance { std::vector<long long> D; };

int computeForG_local(const MDSPInstance& inst, int g) {
    std::vector<int> m(g / 2 + 1, 0);
    for (long long d : inst.D) {
        long long modVal = d % g;
        if (modVal < 0) modVal += g;
        int d_g = static_cast<int>(std::min(modVal, g - modVal));
        if (d_g <= g / 2) m[d_g]++;
    }

    int maxPoints = static_cast<int>(inst.D.size()) + 1;
    for (int N = 1; N <= maxPoints; ++N) {
        std::vector<int> x(g, 0);
        bool found = false;
        auto searchCombination = [&](auto& self, int idx, int remainingSum) -> void {
            if (found) return;
            if (idx == g - 1) {
                x[idx] = remainingSum;
                int sumSame = 0;
                for (int i = 0; i < g; ++i) sumSame += x[i] * (x[i] - 1);
                if (sumSame < 2 * m[0]) return;
                for (int j = 1; j <= g/2; ++j) {
                    int sumDiff = 0;
                    for (int i = 0; i < g; ++i) sumDiff += x[i] * x[(i + j) % g];
                    int required = (g % 2 == 0 && j == g/2) ? 2 * m[j] : m[j];
                    if (sumDiff < required) return;
                }
                found = true;
                return;
            }
            for (int val = 0; val <= remainingSum; ++val) {
                x[idx] = val;
                self(self, idx + 1, remainingSum - val);
                if (found) return;
            }
        };
        searchCombination(searchCombination, 0, N);
        if (found) return N;
    }
    return maxPoints;
}

std::vector<int> transform_Dg(const std::vector<long long>& D, int g) {
    std::vector<int> Dg;
    for (auto d : D) {
        long long modVal = d % g;
        if (modVal < 0) modVal += g;
        int dg = static_cast<int>(std::min(modVal, g - modVal));
        Dg.push_back(dg);
    }
    return Dg;
}

int computeLowerBound_local(const MDSPInstance& inst) {
    int n2 = computeForG_local(inst, 2);
    int n3 = computeForG_local(inst, 3);
    int n5 = computeForG_local(inst, 5);
    return std::max({n2, n3, n5});
}

int main(int argc, char** argv) {
    // default input
    std::vector<long long> D = {1,2,3,4,5};
    int g = 3;

    if (argc >= 2) {
        // parse first arg as comma-separated D values
        std::string s = argv[1];
        D.clear();
        std::stringstream ss(s);
        long long v;
        while (ss >> v) {
            D.push_back(v);
            if (ss.peek() == ',' || ss.peek() == ' ') ss.ignore();
        }
    }
    if (argc >= 3) g = std::stoi(argv[2]);

    MDSPInstance inst{D};
    std::cout << "Input D: {";
    for (size_t i=0;i<D.size();++i){ if (i) std::cout<<","; std::cout<<D[i]; }
    std::cout << "}, g=" << g << "\n";

    auto Dg = transform_Dg(D, g);
    std::cout << "Transformed D_g: {";
    for (size_t i=0;i<Dg.size();++i){ if (i) std::cout<<","; std::cout<<Dg[i]; }
    std::cout << "}\n";

    std::vector<int> m(g/2+1,0);
    for (int v : Dg) if (v>=0 && v<=g/2) m[v]++;
    std::cout << "Frequencies:\n";
    for (size_t j=0;j<m.size();++j) std::cout << "  m_"<<j<<" = "<<m[j]<<"\n";

    int ng = computeForG_local(inst, g);
    std::cout << "Computed n_"<<g<<" = "<<ng<<"\n";

    int finalL = computeLowerBound_local(inst);
    std::cout << "Final lower bound L = max(n2,n3,n5) = "<<finalL<<"\n";

    return 0;
}
