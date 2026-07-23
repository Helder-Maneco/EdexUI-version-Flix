#ifndef MEM_STATS_HPP
#define MEM_STATS_HPP

#include <string>
#include <cstdint>
#include <map>
#include <algorithm>
#include <fstream>

struct MemStats {
    uint64_t total = 0;
    uint64_t available = 0;
    uint64_t used = 0;
    double usagePercent = 0.0;
    
    uint64_t swapTotal = 0;
    uint64_t swapUsed = 0;
    double swapUsagePercent = 0.0;
    
    std::string unit = "MB";
};

class MemoryMonitor {
public:
    MemoryMonitor();//Construtor simples
    
    MemStats update();//Só métodos públicos
    
    static std::string formatBytes(uint64_t bytes);

private:
    static constexpr int REFRESH_INTERVAL_MS = 500;
    std::map<std::string, uint64_t> parseMemInfo();
};

#endif // MEM_STATS_HPP
