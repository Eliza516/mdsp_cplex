#pragma once
#include "core/Types.h"
#include <string>

// Reads instance text files in format:
// line 1: k
// line 2: d_1 d_2 ... d_k
class MDSPReader {
public:
    static MDSPInstance readFromFile(const std::string& path);
};
