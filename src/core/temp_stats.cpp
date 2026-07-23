#include "temp_stats.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

TempMonitor::TempMonitor() {}

std::vector<std::filesystem::path> TempMonitor::findSensorPaths() {
    std::vector<std::filesystem::path> paths;
    auto hwmonDir = std::filesystem::path("/sys/class/hwmon");
    
    if (!std::filesystem::exists(hwmonDir)) {
        return paths;
    }

    for (const auto& entry : std::filesystem::directory_iterator(hwmonDir)) {
        if (entry.is_directory()) {
            paths.push_back(entry.path());
        }
    }

    return paths;
}

float TempMonitor::parseTemperatureValue(const std::string& content) {
    try {
        size_t dotPos = content.find('.');
        if (dotPos != std::string::npos) {
            return std::stof(content) / 1000.0f;
        }
        return 0.0f;
    } catch (...) {
        return 0.0f;
    }
}

TempStats TempMonitor::update() {
    TempStats stats{};
    auto sensorPaths = findSensorPaths();

    for (const auto& path : sensorPaths) {
        auto tempFile = path / "temp1_input";
        if (!std::filesystem::exists(tempFile)) continue;

        std::ifstream file(tempFile);
        if (!file.is_open()) continue;

        std::string line;
        std::getline(file, line);
        float tempCelsius = parseTemperatureValue(line);

        Sensor sensor;
        sensor.name = path.stem().string();
        sensor.temperature = tempCelsius;
        sensor.isOverheating = tempCelsius > sensor.threshold;
        
        stats.sensors.push_back(sensor);
        stats.maxTemperature = std::max(stats.maxTemperature, tempCelsius);

        if (sensor.isOverheating) {
            stats.systemOverheated = true;
        }
    }

    if (!stats.sensors.empty()) {
        float sum = 0.0f;
        for (auto& s : stats.sensors) {
            sum += s.temperature;
        }
        stats.averageTemperature = sum / stats.sensors.size();
    }

    // Atualiza cache interno
    cachedStats = stats;

    return stats;
}

std::string TempMonitor::getThermalWarning() const {
//Usa CACHED_STATS do objeto THIS, não precisa passar params
    if (cachedStats.systemOverheated) {
        return "SYSTEM OVERHEATED! MAX: " + 
               std::to_string(static_cast<int>(cachedStats.maxTemperature)) + "°C";
    }
    return "All temperatures normal";
}
