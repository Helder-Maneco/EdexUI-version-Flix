#include "socket_server.hpp"
#include <arpa/inet.h>
#include <cstring>

SocketServer::SocketServer(int port) : server_fd(-1) {}

SocketServer::~SocketServer() {
    stop();
    if (server_fd >= 0) {
        close(server_fd);
    }
}

void SocketServer::start() {
    if (running.load()) return;
    
    server_fd = createSocket(18080);  // Usa a porta definida no construtor se preferir
    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }
    
    running.store(true);
    accept_thread = std::thread(&SocketServer::acceptLoop, this);
    std::cout << "🔌 Socket server started on port 18080\n";
}

void SocketServer::stop() {
    running.store(false);
    
    if (accept_thread.joinable()) {
        accept_thread.join();
    }
    
    if (server_fd >= 0) {
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
    }
    
    std::cout << " Socket server stopped\n";
}

int SocketServer::createSocket(int port) {
    struct sockaddr_in address;
    int opt = 1;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }
    
    return server_fd;
}

void SocketServer::acceptLoop() {
    while (running.load()) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            continue;
        }
        
        handleClient(client_fd);
    }
}

void SocketServer::handleClient(int client_fd) {
    char buffer[4096] = {0};
    
    read(client_fd, buffer, sizeof(buffer));
    std::cout << "Client message: " << buffer << std::endl;
    
    std::string response = "Edex-CLI Server v0.1.0 OK";
    send(client_fd, response.c_str(), response.length(), 0);
    
    close(client_fd);
}
