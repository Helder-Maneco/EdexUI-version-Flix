#ifndef DISK_STATS_HPP
#define DISK_STATS_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>      // ← Para std::clamp se precisar depois
#include <filesystem>

// ✅ Move 'Partition' pra fora para evitar problemas de escopo no LSP
struct Partition {
    std::string mountPoint = "";
    std::string device = "";

    uint64_t total = 0;
    uint64_t used = 0;
    uint64_t free = 0;
    double usagePercent = 0.0;

    uint64_t readOps = 0;
    uint64_t writeOps = 0;
    double readSpeedMBps = 0.0;
    double writeSpeedMBps = 0.0;
};

struct DiskStats {
    std::vector<Partition> partitions{};
    uint64_t totalSystemSpace = 0;
    uint64_t usedSystemSpace = 0;
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
    
    // ✅ DECLARAÇÃO EXATA — Mesma assinatura no header e .cpp
    uint64_t getUsagePercent(uint64_t used, uint64_t total);
    
    std::string parseDiskInfo(const std::string& devicePath);
};

#endif // DISK_STATS_HPP
