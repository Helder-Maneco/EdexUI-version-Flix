#include <cstdint>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include "mem_stats.hpp"

MemoryMonitor::MemoryMonitor() = default;

std::map<std::string, uint64_t> MemoryMonitor::parseMeInfo()
{
    std::map<std::string, uint64_t> data;
    std::ifstream info("/proc/meminfo");

    if (!info.is_open())
    {
	return data;
    }
}

std::string line;
    while (std::getline(info, line))
    {
	std::istringstream iss(line)
	std::string
    }
