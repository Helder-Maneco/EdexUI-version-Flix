// 📄 src/ui/renderer.cpp - EdexUI CLI RENDERER FINAL
#include "renderer.hpp"
#include "../core/system_info.hpp"
#include <iostream>
#include <cstdio>
#include <thread>
#include <algorithm>

static constexpr int COLOR_DARK = 1;     // pair ID
static constexpr int COLOR_LIGHT = 2;    // pair ID
static constexpr int COLOR_HEADER = 3;   // pair ID (nova cor)
static constexpr int COLOR_DIM = 4;      // pair ID

UIRenderer::UIRenderer(int w, int h) : width(w), height(h) {}

UIRenderer::~UIRenderer() {
    shutdown();
}

bool UIRenderer::init() {
    if (!initscr()) {
        std::cerr << "Failed to initialize ncurses\n";
        return false;
    }
    
    cbreak();
    noecho();
    curs_set(0);                    // Esconde cursor
    
    start_color();
    init_pair(COLOR_DARK, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_LIGHT, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_HEADER, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_DIM, COLOR_CYAN, COLOR_BLACK);
    
    initialized = true;
    return true;
}

void UIRenderer::render(const FullSystemStats& stats, const SystemInfo& sysinfo) {
    if (!initialized) return;
    
    clearScreen(sysinfo);
    
    // ───────── Header Panel ─────────
    drawText(2, 0, sysinfo.hostname + " – " + sysinfo.username + " | " + sysinfo.uptime, COLOR_LIGHT);
    
    // ───────── CPU Section ─────────
    drawText(2, 3, "CPU Usage", COLOR_DIM);
    printUsageBar(stats.cpu.usagePercent, 30, 4);
    
    // ───────── Memory Section ─────────
    drawText(2, 6, "Memory Usage", COLOR_DIM);
    printUsageBar(stats.memory.usagePercent, 30, 7);
    
    // ───────── Network Section ─────────
    double rxMB = stats.network.totalRxBytes / (1024.0 * 1024.0);
    double txMB = stats.network.totalTxBytes / (1024.0 * 1024.0);
    char netBuf[80];
    snprintf(netBuf, sizeof(netBuf), "%.1f MB RX | %.1f MB TX", rxMB, txMB);
    drawText(35, 6, netBuf, COLOR_LIGHT);
    
    // ───────── Disk Section ─────────
    double diskPct = !stats.disk.partitions.empty() ? 
                     stats.disk.partitions[0].usagePercent : 0.0;
    drawText(2, 9, "Disk Usage", COLOR_DIM);
    printUsageBar(diskPct, 30, 10);
    
    // ───────── Temperature Section ─────────
    char tempBuf[32];
    const char* tempStatus = stats.temperature.systemOverheated ? "(HOT)" : "";
    snprintf(tempBuf, sizeof(tempBuf), "%.1f°C %s", 
             stats.temperature.averageTemperature, tempStatus);
    drawText(35, 9, tempBuf, COLOR_LIGHT);
    
    // ───────── Footer Help ─────────
    attron(A_BOLD);
    drawText(2, height - 3, "[q] Quit · [r] Refresh · [←→] Tabs", COLOR_DIM);
    attroff(A_BOLD);
    
    refresh();
    timeout(100);
    
    int key = getch();
    timeout(-1);
    
    if (key == 'q' || key == 'Q') {
        shutdown();
        exit(0);
    }
}

//  ADICIONA FUNCIONALIDADE DE BARRA DE PROGRESSO ASCII
void UIRenderer::printUsageBar(double percent, int width, int y) {
    int filled = static_cast<int>((percent / 100.0) * width);
    int empty = width - filled;
    
    if (filled < 0) filled = 0;
    if (empty < 0) empty = 0;
    
    move(y, width + 4);
    
    // Usar characterset ACS (ASCII Compatible)
    attron(COLOR_PAIR(COLOR_LIGHT));
    for (int i = 0; i < filled; i++) addch(ACS_BLOCK);     // █ alternativa ncurses
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    attron(COLOR_PAIR(COLOR_DIM));
    for (int i = 0; i < empty; i++) addch(ACS_HLINE);      // ─ linha horizontal
    attroff(COLOR_PAIR(COLOR_DIM));
    
    // Valor numérico
    char buf[16];
    snprintf(buf, sizeof(buf), "%.1f%%", percent);
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s", buf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
}

void UIRenderer::clearScreen(const SystemInfo& sysinfo) {
    if (!initialized) return;
    
    clear();
    
    // Borda box completa
    int borderY = 0, borderX = 2;
    int borderW = width - 4;
    int borderH = 14;
    
    box(stdscr, ACS_VLINE, ACS_HLINE);
    
    // Título com background
    attron(COLOR_PAIR(COLOR_HEADER));
    mvhline(borderY + 1, borderX, ' ', borderW);
    attroff(COLOR_PAIR(COLOR_HEADER));
}

void UIRenderer::shutdown() {
    if (initialized) {
        endwin();
        initialized = false;
    }
}

void UIRenderer::drawText(int x, int y, const std::string& text, int color) {
    if (!initialized) return;
    
    move(y, x);
    attron(COLOR_PAIR(color));
    addstr(text.c_str());
    attroff(COLOR_PAIR(color));
}
