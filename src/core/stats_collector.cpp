#include "stats_collector.hpp"
#include <iostream>

StatsCollector::StatsCollector() 
    : running(false), collectorThread(nullptr) {}

StatsCollector::~StatsCollector() {
    stop();
}

void StatsCollector::start() {
    if (running.load()) return;
    
    running.store(true);
    collectorThread = std::make_unique<std::thread>(&StatsCollector::collectLoop, this);
    std::cout << " Stats collection started!\n";
}

void StatsCollector::stop() {
    running.store(false);
    if (collectorThread && collectorThread->joinable()) {
        collectorThread->join();
    }
    collectorThread.reset();
    std::cout << " Stats collection stopped.\n";
}

void StatsCollector::collectLoop() {
    while (running.load()) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            
            // Atualiza TODOS OS MONITORS UMA VEZ SO
            cachedCPU = cpu.update();
            cachedMemory = memory.update();
            cachedNetwork = network.update();
            cachedDisk = disk.update();
            cachedTemperature = temperature.update();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_INTERVAL_MS));
    }
}

//  GETTERS AGORA SEM CONST E RETORNAM CACHE!
CPUStats StatsCollector::getCpuData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return cachedCPU;  // Retorna copia do cache, não chama update()
}

MemStats StatsCollector::getMemData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return cachedMemory;
}

NetStats StatsCollector::getNetData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return cachedNetwork;
}

DiskStats StatsCollector::getDiskData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return cachedDisk;
}

TempStats StatsCollector::getTempData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return cachedTemperature;
}

FullSystemStats StatsCollector::getFullStats() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    FullSystemStats full{};
    full.cpu = cachedCPU;
    full.memory = cachedMemory;
    full.network = cachedNetwork;
    full.disk = cachedDisk;
    full.temperature = cachedTemperature;
    full.lastUpdate = std::chrono::system_clock::now();

    return full;
}
