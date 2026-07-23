#ifndef STATSCOLLECTOR_HPP
#define STATSCOLLECTOR_HPP

#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <memory>
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
    std::chrono::system_clock::time_point lastUpdate = std::chrono::system_clock::now();
};

class StatsCollector {
private:
    std::atomic<bool> running{false};
    std::unique_ptr<std::thread> collectorThread;
    mutable std::mutex dataMutex;
    
    static constexpr int REFRESH_INTERVAL_MS = 1000;  
    
//Monitors internos
    CPUMonitor cpu;
    MemoryMonitor memory;
    NetworkMonitor network;
    DiskMonitor disk;
    TempMonitor temperature;
    
//CACHES para evitar atualização dupla!
    CPUStats cachedCPU{};
    MemStats cachedMemory{};
    NetStats cachedNetwork{};
    DiskStats cachedDisk{};
    TempStats cachedTemperature{};

public:
    StatsCollector();
    ~StatsCollector();

    void start();
    void stop();

//Métodos NÃO são const porque precisam lock guard
    CPUStats getCpuData();
    MemStats getMemData();
    NetStats getNetData();
    DiskStats getDiskData();
    TempStats getTempData();

    FullSystemStats getFullStats();

private:
    void collectLoop();
};

#endif // STATSCOLLECTOR_HPP
