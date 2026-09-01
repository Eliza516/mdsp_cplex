#pragma once
#include <string>

struct RunResult {
    double lb = -1.0;
    double ub = -1.0;
    double gapPercent = 100.0;
    double timeSeconds = 0.0;
    bool valid = false;
};

struct ResultRow {
    std::string instanceName;
    int k = 0;
    RunResult nIP, nFEAS, nMAX, IP, FEAS, MAX, tIP, tFEAS, tMAX;
};

class CsvExporter {
public:
    explicit CsvExporter(std::string csvPath);
    bool alreadyHas(const std::string& instanceName) const;
    void writeRow(const ResultRow& row);

private:
    std::string path_;
    void ensureHeaderExists();
};
