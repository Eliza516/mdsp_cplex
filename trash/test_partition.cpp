#include <iostream>
#include <vector>

int main() {
    int files_size = 5;
    int concurrency_ = 2;
    int workers = std::min(concurrency_, files_size);
    std::vector<std::vector<int>> groups(workers);
    size_t base = files_size / workers;
    size_t rem = files_size % workers;
    size_t idx = 0;
    for (int w = 0; w < workers; ++w) {
        size_t chunk = base + (w < static_cast<int>(rem) ? 1 : 0);
        for (size_t j = 0; j < chunk; ++j) {
            groups[w].push_back(idx++);
        }
    }
    for (int w = 0; w < workers; ++w) {
        std::cout << "Group " << w << ": ";
        for (int x : groups[w]) std::cout << x << " ";
        std::cout << "\n";
    }
    return 0;
}
