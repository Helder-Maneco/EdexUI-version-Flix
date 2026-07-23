#ifndef TEMP_STATS_HPP
#define TEMP_STATS_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

struct Sensor {
    std::string name = "";
    float temperature = 0.0f;
    float threshold = 85.0f;
    bool isOverheating = false;
};

struct TempStats {
    std::vector<Sensor> sensors{};
    float maxTemperature = 0.0f;
    float averageTemperature = 0.0f;
    bool systemOverheated = false;
};

class TempMonitor {
public:
    TempMonitor();

    TempStats update();
    std::string getThermalWarning() const;

private:
    static constexpr int REFRESH_INTERVAL_MS = 2000;
    
//Adiciona cache dos últimos valores aqui também
    TempStats cachedStats;
    
    std::vector<std::filesystem::path> findSensorPaths();
    float parseTemperatureValue(const std::string& content);
};

#endif // TEMP_STATS_HPP
