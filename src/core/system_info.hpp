#ifndef SYSTEM_INFO_HPP
#define SYSTEM_INFO_HPP

#include <string>
#include <map>
#include <memory>

struct SystemInfo {
    std::string hostname;
    std::string username;
    std::string kernel;
    std::string os_name;
    std::string os_version;
    std::string uptime;
    std::string shell;
    std::string cpu_model;
    std::string gpu_model;
    std::string resolution;
    std::string desktop_env;
    std::string term_program;
    std::map<std::string, std::string> additional_info;
};

class SystemInfoCollector {
public:
    SystemInfoCollector();
    SystemInfo collectAll();
    
private:
    std::string readFile(const std::string& path);
    std::string getUptime();
    std::string getHostname();
    std::string getUsername();
    std::string getKernelVersion();
    std::string getOSInfo();
    std::string getShell();
    std::string getCpuModel();
    std::string getGpuModel();
    std::string getResolution();
    std::string getDesktopEnv();
};

#endif // SYSTEM_INFO_HPP
