#pragma once
#include <string>
#include <vector>

class BatchRunner {
public:
    BatchRunner(std::string csvOutputPath, double perInstanceTimeLimitSeconds = 3600.0,
                bool verbose = true, std::string algo = "p1");

    void runDirectory(const std::string& dirPath);
    void runFiles(const std::vector<std::string>& filePaths);

private:
    std::string csvOutputPath_;
    double timeLimit_;
    bool verbose_;
    std::string algo_;

    void runOne(const std::string& filePath);
};
