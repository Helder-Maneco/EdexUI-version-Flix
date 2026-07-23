#include "mem_stats.hpp"
#include <sstream>

MemoryMonitor::MemoryMonitor() {}

std::map<std::string, uint64_t> MemoryMonitor::parseMemInfo() {
    std::map<std::string, uint64_t> data;
    std::ifstream info("/proc/meminfo");
    if (!info.is_open()) return data;

    std::string line;
    while (std::getline(info, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        key.pop_back();
        
        uint64_t valueKB = 0;
        iss >> valueKB;
        data[key] = valueKB * 1024;
    }
    return data;
}

MemStats MemoryMonitor::update() {
    MemStats stats{};
    auto memData = parseMemInfo();

    auto memTotalIt = memData.find("MemTotal");
    auto memFreeIt = memData.find("MemAvailable");
    
    if (memTotalIt != memData.end()) {
        stats.total = memTotalIt->second;
        
        if (memFreeIt != memData.end()) {
            stats.available = memFreeIt->second;
        } else {
            stats.available = stats.total / 4; // Fallback seguro lol
        }
        
        stats.used = stats.total - stats.available;
        stats.usagePercent = (stats.total > 0) ? 
            ((static_cast<double>(stats.used) / static_cast<double>(stats.total)) * 100.0) : 0.0;
        stats.usagePercent = std::clamp(stats.usagePercent, 0.0, 100.0);
    }

    if (stats.total >= (1ULL << 30)) {
        stats.unit = "GB";
    } else {
        stats.unit = "MB";
    }

    return stats;
}

std::string MemoryMonitor::formatBytes(uint64_t bytes) {
    if (bytes >= (1ULL << 30)) return std::to_string(bytes >> 30) + " GB";
    if (bytes >= (1ULL << 20)) return std::to_string(bytes >> 20) + " MB";
    return std::to_string(bytes) + " B";
}
