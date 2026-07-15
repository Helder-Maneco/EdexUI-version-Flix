#ifndef CPU_STATS_HPP
#define CPU_STATS_HPP

#include <string>
#include <vector>
#include <array>

struct CPUStats
{
    double usagePercent; // Uso atual %
    double previousUsage; // Cache pra calcular delta
    std::string coreName; // "CPU0", "CPU1", ...
    int idleTime;  // Tempo ocioso acumulado
    int totalCores;
};

class CPUMonitor
{
    private:
	std::array<long long, 6> lastStat; // user, nice, system
	std::array<long long, 6> currentStat; 
	static constexpr int REFRESH_INTERVAL_MS = 500;

    public:
	CPUMonitor();

	 //Lê /proc/stat e calcula percentual de Uso
	 CPUStats update();

	 // Pega dados brutos do arquivo (útil para debug)
	 std::string getRawStats() const;

    private:
	 bool readProcStat();
	 long long calculateTotal(const std::array<long long, 6>& arr);
	 double calculateUsage(long long idleDiff, long long totalTimeDiff);
};

#endif // ! CPU_STATS_HPP
