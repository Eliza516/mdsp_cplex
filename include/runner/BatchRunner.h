#pragma once
#include <string>
#include <vector>

class BatchRunner {
public:
    // START: resource-control additions
    // Added `threads`, `workMemMB` and `concurrency` to control solver resources and batch behavior.
    BatchRunner(std::string csvOutputPath, double perInstanceTimeLimitSeconds = 3600.0,
                bool verbose = true, std::string algo = "p1",
                int threads = 1, int workMemMB = 1024, int concurrency = 1);
    // END: resource-control additions

    void runDirectory(const std::string& dirPath);
    void runFiles(const std::vector<std::string>& filePaths);

private:
    std::string csvOutputPath_;
    double timeLimit_;
    bool verbose_;
    std::string algo_;

    int threads_ = 1;
    int workMemMB_ = 1024;
    int concurrency_ = 1; // reserved for future parallel runs (currently sequential)

    void runOne(const std::string& filePath);
};
