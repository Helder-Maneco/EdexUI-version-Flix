#ifndef STATSCOLLECTOR_HPP
#define STATSCOLLECTOR_HPP

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <map>
#include "cpu_stats.hpp"
#include "mem_stats.hpp"
#include "net_stats.hpp"
#include "disk_stats.hpp"
#include "temp_stats.hpp"

struct FullSystemStats {
    CPUStats cpu;
    MemStats memory;
    NetStats network;
    DiskStats disk;
    TempStats temperature;
    
    std::chrono::system_clock::time_point lastUpdate;
};

class StatsCollector {
private:
    std::atomic<bool> running{false};
    std::unique_ptr<std::thread> collectorThread;
    mutable std::mutex dataMutex;

    // Stats individuais (geridos separadamente)
    CPUMonitor cpu;
    MemoryMonitor memory;
    NetworkMonitor network;
    DiskMonitor disk;
    TempMonitor temperature;

public:
    StatsCollector() = default;
    ~StatsCollector();

    // Inicia coleta em background thread
    void start();

    // Para coleta
    void stop();

    // Getters thread-safe
    CPUStats getCpuData() const;
    MemStats getMemData() const;
    NetStats getNetData() const;
    DiskStats getDiskData() const;
    TempStats getTempData() const;

    // Retorna struct completo com todos os dados
    FullSystemStats getFullStats() const;

private:
    void collectLoop();
};

#endif // STATSCOLLECTOR_HPP
