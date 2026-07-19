#ifndef CPU_STATS_HPP
#define CPU_STATS_HPP

#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>

static constexpr int NUM_CPU_FIELDS = 6;  // ← Define NO HEADER

struct CPUStats {
    double usagePercent = 0.0;
    double previousUsage = 0.0;
    std::string coreName = "global";
    int idleTime = 0;
    int totalCores = 0;
};

class CPUMonitor {
private:
    std::array<long long, NUM_CPU_FIELDS> currentStat{0, 0, 0, 0, 0, 0};
    std::array<long long, NUM_CPU_FIELDS> lastStat{0, 0, 0, 0, 0, 0};
    
    long long totalPrevious = 0;
    long long idlePrevious = 0;
    CPUStats lastStats;
    
public:
    CPUMonitor();
    
    CPUStats update();
    
    bool readProcStat();                          // ← Declara explicitamente
    long long calculateTotal(const std::array<long long, NUM_CPU_FIELDS>& arr);  // ← Mesmo parâmetro
    
    std::string getRawStats() const;
};

#endif // CPU_STATS_HPP
