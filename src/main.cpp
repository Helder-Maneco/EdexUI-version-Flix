#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <ncurses.h> 
// Arquivos incluidos
#include "ui/renderer.hpp"
#include "core/stats_collector.hpp"
#include "ipc/socket_server.hpp"

/*==============|
 * Global Flags |
 ===============|*/
static std::atomic<bool> g_running{true};

//Signal Handler para CTRL+C clean shutdown
void signal_handler(int signum)
{
    std::cout << "\n Shutting down... (signal: " << signum << ")\n";
    g_running.store(false);
}

/*==================|
 * Application Class|
 ===================|*/
class App
{
    private:
        UIRenderer renderer;
        StatsCollector stats;
        SocketServer ipc_server;    
    public:
        App() : stats(), ipc_server(18080) {}         
        void start() {
            std::cout << " Starting Edex-CLI v0.1.0...\n\n";
            
            // Register signals
            signal(SIGINT, signal_handler);
            signal(SIGTERM, signal_handler);

            // Initialize UI (NCurses nao Termbox!)
            if (!renderer.init())
            {
                std::cerr << " Failed to initialize UI renderer\n";
                return ;
            }
            
            // Start background collection
            stats.start();

            std::cout << " Press Ctrl+C to exit\n\n";

            while (g_running.load()) 
            {
                // Get FULL system stats at once (corrigido!)
                auto fullStats = stats.getFullStats();

                // Render complete UI with FullSystemStats
                renderer.render(fullStats);

                // Sleep entre refreshes (valor adicionado!)
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            
            shutdown();
        }
        
    private:
        void shutdown()
        {
            std::cout << " Cleaning up resources...\n";
            
            stats.stop();
            ipc_server.stop();
            renderer.shutdown();
            std::cout << " Shutdown complete!\n";
        }
};

/*===========|
 * Main Entry |
 ============|*/
int main(int argc, char* argv[])
{
    App app;
    app.start();
    
    return 0;
}
