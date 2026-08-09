#include "core/Reader.h"
#include <fstream>
#include <stdexcept>
#include <string>

MDSPInstance MDSPReader::readFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open instance file: " + path);
    }

    int k;
    if (!(in >> k)) {
        throw std::runtime_error("Failed to read k (number of distances) from " + path);
    }
    if (k <= 0) {
        throw std::runtime_error("Invalid number of distances (k <= 0) in " + path);
    }

    MDSPInstance inst;
    inst.D.reserve(static_cast<size_t>(k));
    for (int i = 0; i < k; ++i) {
        long long d;
        if (!(in >> d)) {
            throw std::runtime_error("Expected " + std::to_string(k) +
                                      " distances but file ended early: " + path);
        }
        if (d <= 0) {
            throw std::runtime_error("Distances must be positive integers, got " +
                                      std::to_string(d));
        }
        inst.D.push_back(d);
    }
    return inst;
}
