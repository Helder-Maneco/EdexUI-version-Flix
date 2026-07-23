#include "renderer.hpp"
#include <iostream>
#include <cstdio>
#include <thread>

static constexpr int COLOR_DARK = 1;     // pair ID
static constexpr int COLOR_LIGHT = 2;    // pair ID

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
    curs_set(0);
    
    start_color();

    init_pair(COLOR_DARK, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_LIGHT, COLOR_WHITE, COLOR_BLACK);
    
    initialized = true;
    return true;
}

void UIRenderer::render(const FullSystemStats& stats) {
    if (!initialized) return;
    
    clearScreen(stats);    
    printw(" Edex-CLI System Monitor \n");
    
    // CPU Info
    mvprintw(3, 3, "CPU Usage: ");
    char cpuBuf[32];
    snprintf(cpuBuf, sizeof(cpuBuf), "%.1f%%", stats.cpu.usagePercent);
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s\n", cpuBuf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    // Memory Info
    mvprintw(5, 3, "Memory Usage: ");
    char memBuf[64];
    snprintf(memBuf, sizeof(memBuf), "%.1f%% (%s)", 
             stats.memory.usagePercent, stats.memory.unit.c_str());
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s\n", memBuf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    // Network Info
    double rxMB = stats.network.totalRxBytes / (1024.0 * 1024.0);
    double txMB = stats.network.totalTxBytes / (1024.0 * 1024.0);
    char netBuf[80];
    snprintf(netBuf, sizeof(netBuf), "%.1f MB RX | %.1f MB TX", rxMB, txMB);
    
    mvprintw(7, 3, "Network Total: ");
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s\n", netBuf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    // Disk Info
    double diskPct = !stats.disk.partitions.empty() ? 
                     stats.disk.partitions[0].usagePercent : 0.0;
    char diskBuf[32];
    snprintf(diskBuf, sizeof(diskBuf), "%.1f%%", diskPct);
    
    mvprintw(9, 3, "Disk Usage: ");
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s\n", diskBuf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    // Temp Info
    char tempBuf[48];
    const char* tempStatus = stats.temperature.systemOverheated ? "(HOT)" : "";
    snprintf(tempBuf, sizeof(tempBuf), "%.1f°C %s", 
             stats.temperature.averageTemperature, tempStatus);
    
    mvprintw(11, 3, "Temperature: ");
    attron(COLOR_PAIR(COLOR_LIGHT));
    printw("%s\n", tempBuf);
    attroff(COLOR_PAIR(COLOR_LIGHT));
    
    // Footer instructions
    attron(A_BOLD);
    mvprintw(13, 5, "[q] Quit · [r] Refresh");
    attroff(A_BOLD);
    
    // Linhas finais com loop pra evitar addstr
    move(height - 1, 0);
    for (int i = 0; i < width; i++) {
        addch(' ');
    }
    addch('\n');
    
    refresh();
    timeout(100); // Timeout pro getch()
    
    int key = getch();
    timeout(-1);// Restaura blocking mode
    
    if (key == 'q' || key == 'Q') {
        shutdown();
        exit(0);
    }
}

void UIRenderer::clearScreen(const FullSystemStats& stats) {
    if (!initialized) return;
    
    clear();     
    // Borda box completa
    int borderY = 0, borderX = 2;
    int borderW = width - 4;
    int borderH = 13;
    
    box(stdscr, ACS_VLINE, ACS_HLINE);
    
    // Title bar background
    attron(COLOR_PAIR(COLOR_DARK));
    for (int i = borderX; i < borderX + borderW; i++) {
        mvhline(borderY + 0, i, ' ', 2);
    }
    attroff(COLOR_PAIR(COLOR_DARK));
}

void UIRenderer::shutdown() {
    if (initialized) {
        endwin();
        initialized = false;
    }
}

void UIRenderer::drawText(int x, int y, const std::string& text, int color) {
    if (!initialized) return;
    mvaddstr(y, x, text.c_str());//Usa mvaddstr em vez de addstr
    attroff(COLOR_PAIR(color));
}
