#include "core/Reader.h"
#include <fstream>
#include <stdexcept>
#include <string>

MDSPInstance MDSPReader::readFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open instance file: " + path);
    }
    MDSPInstance inst;
    std::string word;
    std::vector<long long> all_numbers;

    while (in >> word) {
        if (word == "NAME") {
            in >> word; // skip the name string
            continue;
        }
        try {
            all_numbers.push_back(std::stoll(word));
        } catch (...) {
            // Ignore non-integers or throw if you prefer strictness
        }
    }

    if (all_numbers.empty()) {
        throw std::runtime_error("No distances found in " + path);
    }

    // Treat all parsed numeric tokens as distances (do not treat a leading
    // integer as a count). Some instance files include a leading count; if
    // those files actually include the count, remove it from the file instead
    // or adapt this reader intentionally. This change makes the reader
    // consistently interpret every numeric token as a distance value.
    inst.D = all_numbers;

    for (long long d : inst.D) {
        if (d <= 0) {
            throw std::runtime_error("Distances must be positive integers, got " + std::to_string(d));
        }
    }

    return inst;
}
