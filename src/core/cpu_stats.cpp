#include "cpu_stats.hpp"
#include <fstream>
#include <sstream>

CPUMonitor::CPUMonitor() : totalPrevious(0), idlePrevious(0) {}

CPUStats CPUMonitor::update() {
    if (!readProcStat()) {
        return lastStats;
    }

    long long totalTimeCurrent = calculateTotal(currentStat);
    long long idleTimeCurrent = currentStat[3];

    long long totalTimeDiff = totalTimeCurrent - totalPrevious;
    long long idleTimeDiff = idleTimeCurrent - idlePrevious;

    if (totalTimeDiff <= 0 || idleTimeDiff < 0) {
        return lastStats;
    }

    double usagePercent = (1.0 - ((double)idleTimeDiff / (double)totalTimeDiff)) * 100.0;
    usagePercent = std::clamp(usagePercent, 0.0, 100.0);

    totalPrevious = totalTimeCurrent;
    idlePrevious = idleTimeCurrent;
    lastStats.usagePercent = usagePercent;
    lastStats.totalCores = 8;

    return lastStats;
}

bool CPUMonitor::readProcStat() {
    std::ifstream stat("/proc/stat");
    if (!stat.is_open()) {
        return false;
    }

    std::string line;
    getline(stat, line);

    std::istringstream iss(line);
    std::string header;
    iss >> header;

    for (int i = 0; i < NUM_CPU_FIELDS; ++i) {
        iss >> currentStat[i];
    }

    return true;
}

long long CPUMonitor::calculateTotal(const std::array<long long, NUM_CPU_FIELDS>& arr) {
    long long sum = 0;
    for (auto val : arr) {
        sum += val;
    }
    return sum;
}

std::string CPUMonitor::getRawStats() const {
    return "Not implemented yet";
}
