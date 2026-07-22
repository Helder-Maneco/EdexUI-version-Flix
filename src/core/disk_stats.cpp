#include "disk_stats.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/statfs.h>     
#include <map>

static const std::vector<std::string> EXCLUDED_INTERFACES = {"loop", "sr0"};

std::string DiskMonitor::formatSize(uint64_t bytes) {
    if (bytes >= (1ULL << 30)) {      // GB
        return std::to_string(bytes >> 30) + " GB";
    } else if (bytes >= (1ULL << 20)) {  // MB
        return std::to_string(bytes >> 20) + " MB";
    } else if (bytes >= (1ULL << 10)) {  // KB
        return std::to_string(bytes >> 10) + " KB";
    }
    return std::to_string(bytes) + " B";
}

std::vector<std::string> DiskMonitor::getMountPoints() {
    std::vector<std::string> mounts;
    
    std::ifstream mtab("/proc/mounts");
    if (!mtab.is_open()) {
        return mounts;
    }

    std::string line;
    while (std::getline(mtab, line)) {
        std::istringstream iss(line);
        std::string device, mount, fstype, options;
        
        iss >> device >> mount >> fstype >> options;
        
        // Ignora sistemas de arquivo especiais
        if (fstype == "tmpfs" || fstype == "devtmpfs" || 
            fstype == "proc" || fstype == "sysfs") {
            continue;
        }
        
        mounts.push_back(mount);
    }

    return mounts;
}

// ✅ IMPLEMENTAÇÃO CORRETA — Signatura EXATAMENTE igual ao header
uint64_t DiskMonitor::getUsagePercent(uint64_t used, uint64_t total) {
    if (total == 0) return 0;
    return static_cast<uint64_t>((static_cast<double>(used) / static_cast<double>(total)) * 100.0);
}

std::string DiskMonitor::parseDiskInfo(const std::string& devicePath) {
    std::ifstream stat("/proc/diskstats");
    if (!stat.is_open()) {
        return "";
    }

    std::string line;
    while (std::getline(stat, line)) {
        std::istringstream iss(line);
        std::string devId;
        iss >> devId;
        
        // Procurar o dispositivo (ex: "8 2 nvme0n1p2")
        if (line.find(devicePath) != std::string::npos) {
            return line;
        }
    }

    return "";
}

DiskStats DiskMonitor::update() {
    DiskStats stats{};
    
    // 1. Leitura via statfs (filesystem space)
    for (const auto& mount : getMountPoints()) {
        struct statfs fsInfo;
        if (statfs(mount.c_str(), &fsInfo) == -1) {
            continue;
        }

        // Calcula sizes em bytes
        uint64_t blockSize = fsInfo.f_bsize;
        uint64_t totalBlocks = fsInfo.f_blocks;
        uint64_t freeBlocks = fsInfo.f_bfree;
        uint64_t availableBlocks = fsInfo.f_bavail;
        
        uint64_t totalBytes = totalBlocks * blockSize;
        uint64_t freeBytes = freeBlocks * blockSize;
        uint64_t usedBytes = totalBytes - freeBytes;
        
        // Extrair nome do dispositivo a partir do mount point
        std::string device = "/dev/" + mount.substr(1); // Ex: "/" → "/dev/sda1"
        if (device == "/dev/") device = "/dev/sda1"; // Fallback genérico

        Partition part{
            .mountPoint = mount,
            .device = device,
            .total = totalBytes,
            .used = usedBytes,
            .free = freeBytes,
	    .usagePercent = (totalBytes > 0) ? 
        ((static_cast<double>(usedBytes) / static_cast<double>(totalBytes)) * 100.0) : 0.0,
	    .readOps = 0,
            .writeOps = 0,
            .readSpeedMBps = 0.0,
            .writeSpeedMBps = 0.0
        };

        // Adiciona ao vector final
        stats.partitions.push_back(part);
        
        // Some ao total do sistema
        stats.totalSystemSpace += totalBytes;
        stats.usedSystemSpace += usedBytes;
    }

    return stats;
}
