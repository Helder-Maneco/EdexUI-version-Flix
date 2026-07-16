#ifndef MEM_STATS_HPP
#define MEM_STATS_HPP

#include <string>
#include <cstdint>
#include <map>
#include <algorithm>

struct MemStats {
    uint64_t total;          // Total memória em bytes
    uint64_t available;      // Memória disponível em bytes
    uint64_t used;           // Memória usada em bytes
    double usagePercent;     // Percentual de uso (0.0 - 100.0)
    
    uint64_t swapTotal;      // Swap total
    uint64_t swapUsed;       // Swap usado
    double swapUsagePercent; // Percentual swap
    
    std::string unit;        // "MB" ou "GB" pra exibição
};

class MemoryMonitor {
private:
    static constexpr int REFRESH_INTERVAL_MS = 500;

public:
    MemoryMonitor() = default;
    
    // Lê /proc/meminfo e retorna stats atualizados
    MemStats update();
    
    // Formata bytes pra string legivel (1.5 GB, e por ia..)
    static std::string formatBytes(uint64_t bytes);

private:
    std::map<std::string, uint64_t> parseMemInfo();
};

#endif // MEM_STATS_HPP
