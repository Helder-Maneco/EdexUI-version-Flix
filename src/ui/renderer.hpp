#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <ncurses.h>
#include <string>
#include <vector>
#include "../core/stats_collector.hpp"
#include "../core/system_info.hpp"

class UIRenderer {
private:
    bool initialized = false;
    int width = 80;
    int height = 24;

public:
    UIRenderer(int w = 80, int h = 24);
    ~UIRenderer();
    
    bool init();
    void render(const FullSystemStats& stats, const SystemInfo& sysinfo);
    void clearScreen(const SystemInfo& sysinfo);
    void shutdown();
    
    void printUsageBar(double percent, int width, int y);  // ← ADICIONA AQUI!
    void drawText(int x, int y, const std::string& text, int color);
};

#endif // RENDERER_HPP
