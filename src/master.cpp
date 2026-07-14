#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>

//Arquivos incluidos
#include "UI/renderer.hpp"
#include "Core/stats_collector.hpp"
#include "IPC/socket_server.hpp"

/*==============|
 * Global Flags |
 ===============|*/
static std::atomic<bool> g_running{true};

//Signal Handler para CTRL+C clean shutdown
void signal_handler(int)
{
    std::cout << "\n Shutting down...";
	g_running = false;
}

/*==================|
 * Application Class|
 ===================|*/
class App
{
    private:
	UIRenderer renderer;
	StatsCollector stats;
	SocketSever ipc_server;

    public:
	App() : stats(), ipc_server(18080) {}
	void start() {
	    std::cout << "Starting Edex-CLI v0.1.0...\n\n";
	    signal(SIGINT, signal_handler);
	    signal(SIGTERM, signal_handler);

	    if (!renderer.init())
	    {
		std::cerr << "Failed to initialize Termbox renderer\n";
		return ;
	    }
	    stats.start();

	    std::cout << "Press Ctrl+C to exit\n";

	    while (g_running.load()) 
	    {
		auto cpuData = stats.getCpuData();
		auto memData = stats.getMemData();

		renderer.render(cpuData, memData);

		std::this_thread::sleep_for(std::chrono::milliseconds)
	    }
	    shutdown();
	}
    private:
	void shutdown()
	{
	    std::cout << "Cleaning up resources...\n";

	    stats.stop();
	    ipc_server.stop();
	    renderer.shutdown();
	    std::cout << "Shutdown complete!\n";
	}
};

/*===========|
 *Main Entry |
 ============|*/
int main (int argc, char* argv[])
{
    App app;
    app.start();

    return 0;
}
