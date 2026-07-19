#ifndef MEM_STATS_HPP
#define MEM_STATS_HPP

#include <string>
#include <cstdint>
#include <map>
#include <algorithm>
#include <fstream>

struct MemStats {
    uint64_t total = 0;          // Total memória em bytes
    uint64_t available = 0;      // Memória disponível em bytes
    uint64_t used = 0;           // Memória usada em bytes
    double usagePercent = 0.0;   // Percentual de uso (0.0 - 100.0)
    
    uint64_t swapTotal = 0;      // Swap total
    uint64_t swapUsed = 0;       // Swap usado
    double swapUsagePercent = 0.0; // Percentual swap
    
    std::string unit = "MB";     // "MB" ou "GB" pra exibição
};

class MemoryMonitor {
private:
    static constexpr int REFRESH_INTERVAL_MS = 500;

public:
    MemoryMonitor() = default;
    
    // Lê /proc/meminfo e retorna stats atualizados
    MemStats update();
    
    // Formata bytes pra string legivel (1.5 GB, etc)
    static std::string formatBytes(uint64_t bytes);

private:
    std::map<std::string, uint64_t> parseMemInfo();
};

#endif // MEM_STATS_HPP
