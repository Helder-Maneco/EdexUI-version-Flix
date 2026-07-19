#ifndef TEMP_STATS_HPP
#define TEMP_STATS_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

struct TempStats {
    struct Sensor {
        std::string name;      // Tipo do sensor (acpitz, coretemp_0)
        float temperature;     // Temperatura em °C
        float threshold;       // Limite de segurança (°C)
        bool isOverheating;    // True se ultrapassar limite
    };

    std::vector<Sensor> sensors;
    float maxTemperature;     // Maior temperatura ativa
    float averageTemperature; // Temperatura média de todos
    bool systemOverheated;    // True se qualquer sensor superaquecido

public:
    static constexpr float SAFETY_THRESHOLD_CELSIUS = 85.0f;

    // Verifica status térmico do sistema
    void analyzeThermalStatus();

private:
    std::filesystem::path hwmonPath;
};

class TempMonitor {
private:
    TempStats cachedStats;
    static constexpr int REFRESH_INTERVAL_MS = 2000; // Menos crítico lol

public:
    TempMonitor() = default;
    
    // Varre /sys/class/hwmon/* e coleta temperaturas
    TempStats update();
    
    // Retorna mensagem de alerta se necessário
    std::string getThermalWarning() const;

private:
    std::vector<std::filesystem::path> findSensorPaths();
    float parseTemperatureValue(const std::string& content);
    std::string extractSensorName(const std::filesystem::path& path);
};

#endif // TEMP_STATS_HPP
