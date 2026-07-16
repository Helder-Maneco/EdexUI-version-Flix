#ifndef DISK_STATS_HPP
#define DISK_STATS_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <filesystem>

// ✅ MOVE 'Partition' PRA FORA DE 'DiskStats'
struct Partition {
    std::string mountPoint;
    std::string device;

    uint64_t total;
    uint64_t used;
    uint64_t free;
    double usagePercent;

    uint64_t readOps;
    uint64_t writeOps;
    double readSpeedMBps;
    double writeSpeedMBps;
};

struct DiskStats {
    std::vector<Partition> partitions;
    uint64_t totalSystemSpace;
    uint64_t usedSystemSpace;
};

class DiskMonitor {
public:
    DiskMonitor() = default;

    // Lê /proc/diskstats + df para obter stats completos
    DiskStats update();

    // Formata bytes para string legível com unidade adequada
    static std::string formatSize(uint64_t bytes);

private:
    std::unordered_map<std::string, Partition> lastReadings;
    static constexpr int REFRESH_INTERVAL_MS = 1000;

    std::vector<std::string> getMountPoints();
    uint64_t getUsagePercent(uint64_t used, uint64_t total);
};

#endif // DISK_STATS_HPP
