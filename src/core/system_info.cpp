// 📄 src/core/system_info.cpp - IMPLEMENTAÇÃO FINAL COMPLETA
#include "system_info.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unistd.h>      // ← PARA gethostname()
#include <netdb.h>       // ← PARA gethostbyname() se precisar

SystemInfoCollector::SystemInfoCollector() {}

std::string SystemInfoCollector::readFile(const std::string& path) {
    try {
        if (!std::filesystem::exists(path)) return "";
        
        std::ifstream file(path);
        if (!file.is_open()) return "";
        
        std::string content;
        std::getline(file, content);
        file.close();
        return content;
    } catch (...) {
        return "";
    }
}

std::string SystemInfoCollector::getHostname() {
    char hostname[256];
    
    // Tenta gethostname primeiro (mais seguro)
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        std::string result(hostname);
        size_t pos = result.find('.');
        if (pos != std::string::npos) {
            result = result.substr(0, pos);
        }
        return result;
    }
    
    // Fallback para /etc/hostname
    auto content = readFile("/etc/hostname");
    if (!content.empty()) {
        return content;
    }
    
    return "unknown";
}

std::string SystemInfoCollector::getUsername() {
    const char* user = getenv("USER");
    return user ? user : "unknown";
}

std::string SystemInfoCollector::getKernelVersion() {
    auto content = readFile("/proc/version");
    if (content.empty()) return "N/A";
    
    size_t spacePos = content.find(' ');
    if (spacePos != std::string::npos) {
        content = content.substr(spacePos + 1);
        spacePos = content.find(' ');
        if (spacePos != std::string::npos) {
            content = content.substr(0, spacePos);
        }
    }
    return content;
}

std::string SystemInfoCollector::getOSInfo() {
    auto os_release = readFile("/etc/os-release");
    
    if (!os_release.empty()) {
        std::istringstream iss(os_release);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.find("NAME=") == 0) {
                line.erase(line.find("=") + 1);
                line = line.length() > 0 && line.back() == '"' ? 
                       line.substr(0, line.length() - 1) : line;
                return line;
            }
        }
    }
    
    return "Linux";
}

std::string SystemInfoCollector::getShell() {
    const char* shell = getenv("SHELL");
    return shell ? shell : "/bin/bash";
}

std::string SystemInfoCollector::getCpuModel() {
    auto content = readFile("/proc/cpuinfo");
    if (content.empty()) return "Unknown CPU";
    
    size_t modelPos = content.find("model name");
    if (modelPos != std::string::npos) {
        std::istringstream iss(content.substr(modelPos));
        std::string line;
        getline(iss, line);
        size_t colonPos = line.find(":");
        if (colonPos != std::string::npos) {
            std::string result = line.substr(colonPos + 1);
            while (!result.empty() && result[0] == ' ') {
                result.erase(0, 1);
            }
            return result;
        }
    }
    return "Unknown CPU";
}

std::string SystemInfoCollector::getResolution() {
    FILE* pipe = popen("xrandr 2>/dev/null | grep '*' | head -1", "r");
    if (pipe) {
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string result(buffer);
            pclose(pipe);
            
            size_t widthPos = result.find('x');
            if (widthPos != std::string::npos) {
                return result.substr(0, widthPos);
            }
        }
        pclose(pipe);
    }
    
    return "1920x1080";
}

std::string SystemInfoCollector::getDesktopEnv() {
    const char* desk = getenv("XDG_SESSION_DESKTOP");
    if (desk && strlen(desk) > 0) return desk;
    
    const char* gnome = getenv("GNOME_DESKTOP_SESSION_ID");
    if (gnome && strlen(gnome) > 0) return "GNOME";
    
    const char* plasma = getenv("KDE_FULL_SESSION");
    if (plasma && strlen(plasma) > 0) return "Plasma";
    
    return "Wayland";
}

std::string SystemInfoCollector::getUptime() {
    std::ifstream uptime_file("/proc/uptime");
    float seconds = 0.0f;
    
    if (uptime_file >> seconds) {
        int totalSeconds = static_cast<int>(seconds);
        int days = totalSeconds / 86400;
        int hours = (totalSeconds % 86400) / 3600;
        int mins = (totalSeconds % 3600) / 60;
        
        char buf[32];
        snprintf(buf, sizeof(buf), "%dd %dh %dm", days, hours, mins);
        return std::string(buf);
    }
    return "N/A";
}

SystemInfo SystemInfoCollector::collectAll() {
    SystemInfo info;
    
    info.hostname = getHostname();
    info.username = getUsername();
    info.kernel = getKernelVersion();
    info.os_name = getOSInfo();
    info.uptime = getUptime();
    info.shell = getShell();
    info.cpu_model = getCpuModel();
    info.resolution = getResolution();
    info.desktop_env = getDesktopEnv();
    
    info.additional_info["theme"] = "EdexUI Theme";
    info.additional_info["zoom"] = "100%";
    info.additional_info["terminal"] = getenv("TERM") ? getenv("TERM") : "alacritty";
    
    return info;
}
