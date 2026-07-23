#ifndef NET_STATS_HPP
#define NET_STATS_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

struct Connection {
    std::string interface = "";
    uint64_t rxBytes = 0;
    uint64_t txBytes = 0;
    double rxSpeedMbps = 0.0;
    double txSpeedMbps = 0.0;
    uint32_t rxErrors = 0;
    uint32_t txDrops = 0;
};

struct NetStats {
    std::vector<Connection> interfaces{};
    uint64_t totalRxBytes = 0;
    uint64_t totalTxBytes = 0;
};

//Move RawStat
struct NetworkMonitorRawStat {
    uint64_t rxBytes = 0;
    uint64_t rxErrors = 0;
    uint64_t rxDrops = 0;
    uint64_t txBytes = 0;
    uint64_t txErrors = 0;
    uint64_t txDrops = 0;
};

class NetworkMonitor {
public:
    NetworkMonitor();
    
    NetStats update();
    std::vector<std::string> getActiveInterfaces() const;

private:
    static constexpr int REFRESH_INTERVAL_MS = 500;
    //Usa nome externo em vez de RawStat interno
    NetworkMonitorRawStat readInterface(const std::string& iface);
};

#endif // NET_STATS_HPP
