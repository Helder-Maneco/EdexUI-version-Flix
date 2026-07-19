#ifndef NET_STATS_HPP
#define NET_STATS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

struct Connection {
    std::string interface;      // eth0, lo, ens33...
    uint64_t rxBytes;           // Bytes recebidos
    uint64_t txBytes;           // Bytes enviados
    double rxSpeedMbps;         // Velocidade RX atual Mbps
    double txSpeedMbps;         // Velocidade TX atual Mbps
    uint32_t rxErrors;          // Erros na recepção
    uint32_t txDrops;           // Pacotes descartados envio
};

struct NetStats {
    std::vector<Connection> interfaces;
    uint64_t totalRxBytes;          // Total acumulado RX
    uint64_t totalTxBytes;          // Total acumulado TX
};

class NetworkMonitor {
private:
    std::unordered_map<std::string, Connection> lastReadings;
    static constexpr int REFRESH_INTERVAL_MS = 500;

public:
    NetworkMonitor() = default;
    
    // Lê /proc/net/dev e calcula velocidade atual
    NetStats update();
    
    // Retorna apenas interfaces principais (ignora loopback/lo)
    std::vector<std::string> getActiveInterfaces() const;

private:
    struct RawStat {
        uint64_t rxBytes;
        uint64_t rxErrors;
        uint64_t rxDrops;
        uint64_t txBytes;
        uint64_t txErrors;
        uint64_t txDrops;
    };
    
    RawStat readInterface(const std::string& iface);
};

#endif // NET_STATS_HPP
