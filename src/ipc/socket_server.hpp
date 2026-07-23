#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <string>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

class SocketServer {
private:
    int server_fd;
    std::atomic<bool> running{false};
    std::thread accept_thread;
    
public:
    SocketServer(int port = 18080);
    ~SocketServer();
    
    void start();
    void stop();
    
    bool isRunning() const { return running.load(); }

private:
    void acceptLoop();
    int createSocket(int port);
    void handleClient(int client_fd);
};

#endif // SOCKET_SERVER_HPP
