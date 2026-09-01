#include "runner/CsvExporter.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <mutex>

CsvExporter::CsvExporter(std::string csvPath) : path_(std::move(csvPath)) {
    ensureHeaderExists();
}

void CsvExporter::ensureHeaderExists() {
    std::ifstream check(path_);
    if (!check.good()) {
        std::ofstream out(path_);
        out << "Instances,|D|,nIP_LB,nIP_UB,nIP_gap,nIP_time,nFEAS_LB,nFEAS_UB,nFEAS_gap,nFEAS_time,IP_LB,IP_UB,IP_gap,IP_time,FEAS_LB,FEAS_UB,FEAS_gap,FEAS_time,tIP_LB,tIP_UB,tIP_gap,tIP_time,tFEAS_LB,tFEAS_UB,tFEAS_gap,tFEAS_time\n";
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

static void printResult(std::ofstream& out, const RunResult& res, bool isLast) {
    if (!res.valid) {
        out << "-,-,-,-" << (isLast ? "" : ",");
        return;
    }
    if (res.lb < 0 || res.lb > 1e20) out << "-,";
    else out << std::fixed << std::setprecision(1) << res.lb << ",";

    if (res.ub < 0 || res.ub > 1e20) out << "-,";
    else out << std::fixed << std::setprecision(1) << res.ub << ",";

    out << std::fixed << std::setprecision(1) << res.gapPercent << ","
        << std::fixed << std::setprecision(1) << res.timeSeconds << (isLast ? "" : ",");
}

void CsvExporter::writeRow(const ResultRow& row) {
    static std::mutex csv_mutex;
    std::lock_guard<std::mutex> guard(csv_mutex);

    std::ofstream out(path_, std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Could not append to CSV file: " + path_);
    }

    out << row.instanceName << "," << row.k << ",";
    printResult(out, row.nIP, false);
    printResult(out, row.nFEAS, false);
    printResult(out, row.IP, false);
    printResult(out, row.FEAS, false);
    printResult(out, row.tIP, false);
    printResult(out, row.tFEAS, true);
    out << "\n";

    out.flush();
}
