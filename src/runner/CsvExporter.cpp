#include "runner/CsvExporter.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

CsvExporter::CsvExporter(std::string csvPath) : path_(std::move(csvPath)) {
    ensureHeaderExists();
}

void CsvExporter::ensureHeaderExists() {
    std::ifstream check(path_);
    if (!check.good()) {
        std::ofstream out(path_);
        out << "Instance,|D|,l,u,B,LB,UB,Gap(%),Time(s),Status\n";
    }
}

bool CsvExporter::alreadyHas(const std::string& instanceName) const {
    std::ifstream in(path_);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto comma = line.find(',');
        std::string firstCol = (comma == std::string::npos) ? line : line.substr(0, comma);
        if (firstCol == instanceName) return true;
    }
    return false;
}

void CsvExporter::writeRow(const ResultRow& row) {
    std::ofstream out(path_, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Could not append to CSV file: " + path_);
    }

    out << row.instanceName << ","
        << row.k << ","
        << row.l << ","
        << row.u << ","
        << row.B << ",";

    if (row.lb < 0) out << "-,";
    else out << std::fixed << std::setprecision(1) << row.lb << ",";

    if (row.ub < 0) out << "-,";
    else out << std::fixed << std::setprecision(1) << row.ub << ",";

    out << std::fixed << std::setprecision(2) << row.gapPercent << ","
        << std::fixed << std::setprecision(2) << row.timeSeconds << ","
        << row.status << "\n";

    out.flush();
}
