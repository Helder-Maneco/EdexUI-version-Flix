#include "net_stats.hpp"
#include <fstream>
#include <sstream>

NetworkMonitor::NetworkMonitor() {}

//Nome correto do struct (externo)
NetworkMonitorRawStat NetworkMonitor::readInterface(const std::string& iface) {
    NetworkMonitorRawStat stat{};
    std::ifstream dev("/proc/net/dev");
    if (!dev.is_open()) return stat;

    std::string line;
    while (std::getline(dev, line)) {
        if (line.find(iface + ":") == std::string::npos) continue;

        std::istringstream iss(line);
        std::string dummy;
        iss >> dummy 
            >> stat.rxBytes 
            >> stat.rxErrors 
            >> stat.rxDrops 
            >> stat.txBytes 
            >> stat.txErrors 
            >> stat.txDrops;
    }

    return stat;
}

NetStats NetworkMonitor::update() {
    NetStats stats{};
    
    std::ifstream dev("/proc/net/dev");
    if (!dev.is_open()) return stats;

    std::string line;
    while (std::getline(dev, line)) {
        if (line.empty() || line.find(":") == std::string::npos) continue;

        size_t colonPos = line.find(":");
        std::string ifaceName = line.substr(0, colonPos);
        
        // Ignora loopback
        if (ifaceName.find("lo") != std::string::npos) continue;
        
        NetworkMonitorRawStat currentStat = readInterface(ifaceName);
        Connection conn;
        conn.interface = ifaceName;
        conn.rxBytes = currentStat.rxBytes;
        conn.txBytes = currentStat.txBytes;
        conn.rxErrors = currentStat.rxErrors;
        conn.txDrops = currentStat.txDrops;
        
        stats.interfaces.push_back(conn);
        stats.totalRxBytes += currentStat.rxBytes;
        stats.totalTxBytes += currentStat.txBytes;
    }

    return stats;
}

std::vector<std::string> NetworkMonitor::getActiveInterfaces() const {
    return {"eth0", "wlan0"}; // Fallback pra MVP
}
