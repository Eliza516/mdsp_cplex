#include <iostream>
#include <vector>
#include <algorithm>

bool checkConstraints(const std::vector<int>& x, int g, const std::vector<int>& m) {
    int sumSame = 0;
    for (int i = 0; i < g; ++i) {
        sumSame += x[i] * (x[i] - 1);
    }
    if (sumSame < 2 * m[0]) return false;

    for (int j = 1; j <= g / 2; ++j) {
        int sumDiff = 0;
        for (int i = 0; i < g; ++i) {
            sumDiff += x[i] * x[(i + j) % g];
        }
        int required = (g % 2 == 0 && j == g / 2) ? 2 * m[j] : m[j];
        if (sumDiff < required) return false;
    }
    return true;
}

int computeForG(const std::vector<long long>& D, int g) {
    std::vector<int> m(g / 2 + 1, 0);
    for (long long d : D) {
        long long modVal = d % g;
        if (modVal < 0) modVal += g;
        int d_g = static_cast<int>(std::min(modVal, g - modVal));
        if (d_g <= g / 2) {
            m[d_g]++;
        }
    }

    int maxPoints = D.size() + 1;
    for (int N = 1; N <= maxPoints; ++N) {
        std::vector<int> x(g, 0);
        bool found = false;

        auto searchCombination = [&](auto& self, int idx, int remainingSum) -> void {
            if (found) return;
            if (idx == g - 1) {
                x[idx] = remainingSum;
                if (checkConstraints(x, g, m)) found = true;
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

int main() {
    std::vector<long long> D = {25, 28, 168, 172, 200};
    int n2 = computeForG(D, 2);
    int n3 = computeForG(D, 3);
    int n5 = computeForG(D, 5);
    std::cout << "n2 = " << n2 << ", n3 = " << n3 << ", n5 = " << n5 << std::endl;
    std::cout << "LB = " << std::max({n2, n3, n5}) << std::endl;
    return 0;
}
