#pragma once
#include <string>

struct ResultRow {
    std::string instanceName;
    int k = 0;
    int l = 0;
    int u = 0;
    long long B = 0;
    double lb = -1.0;
    double ub = -1.0;
    double gapPercent = 100.0;
    double timeSeconds = 0.0;
    std::string status;
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
