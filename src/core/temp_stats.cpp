#include "temp_stats.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

TempMonitor::TempMonitor() {}

std::vector<std::filesystem::path> TempMonitor::findSensorPaths() {
    std::vector<std::filesystem::path> paths;
    
    // Thermal zone (comum no Arch)
    auto thermalDir = std::filesystem::path("/sys/class/thermal");
    if (std::filesystem::exists(thermalDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(thermalDir)) {
            if (entry.is_directory()) {
                paths.push_back(entry.path());
            }
        }
    }
    
    return paths;
}

float TempMonitor::parseTemperatureValue(const std::string& content) {
    try {
        float tempCelsius = std::stof(content);
        
        // Thermal zones usam miliCelsius → divide por 1000
        if (content.find('.') == std::string::npos) {
            tempCelsius /= 1000.0f;
        }
        
        return tempCelsius;
    } catch (...) {
        return 0.0f;
    }
}

TempStats TempMonitor::update() {
    TempStats stats{};
    auto sensorPaths = findSensorPaths();

    for (const auto& path : sensorPaths) {
        std::ifstream file(path / "temp1_input");
        std::string line;
        
        // Tenta temp1_input primeiro, senão tenta input
        if (!file.is_open()) {
            file.open(path / "input");
        }
        
        if (!file.is_open()) continue;
        
        getline(file, line);
        file.close();
        
        if (line.empty()) continue;
        
        float tempCelsius = parseTemperatureValue(line);
        
        if (tempCelsius > 0) {
            Sensor sensor;
            sensor.name = path.filename().string();
            sensor.temperature = tempCelsius;
            sensor.isOverheating = tempCelsius > 85.0f;
            
            stats.sensors.push_back(sensor);
            stats.maxTemperature = std::max(stats.maxTemperature, tempCelsius);
            
            if (sensor.isOverheating) {
                stats.systemOverheated = true;
            }
        }
    }

    // Fallback se nenhum sensor encontrado
    if (stats.sensors.empty()) {
        Sensor fallback;
        fallback.name = "system";
        fallback.temperature = 35.0f;
        fallback.isOverheating = false;
        stats.sensors.push_back(fallback);
        stats.maxTemperature = fallback.temperature;
    }

    if (!stats.sensors.empty()) {
        float sum = 0.0f;
        for (auto& s : stats.sensors) {
            sum += s.temperature;
        }
        stats.averageTemperature = sum / stats.sensors.size();
    }

    cachedStats = stats;
    return stats;
}

std::string TempMonitor::getThermalWarning() const {
    if (cachedStats.systemOverheated) {
        return " SYSTEM OVERHEATED! MAX: " + 
               std::to_string(static_cast<int>(cachedStats.maxTemperature)) + "°C";
    }
    if (cachedStats.sensors.empty()) {
        return " No temperature sensors available";
    }
    return " All temperatures normal";
}
