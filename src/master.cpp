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
	App() : stats(), ipc_server(18080)
} 
