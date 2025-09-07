//
// Created by andres on 7/9/25.
//

#include "SocketServer.h"

SocketServer::SocketServer(int port) : serverPort(port) {}

SocketServer::~SocketServer() {
    stop();
}

void SocketServer::start() {
    serverRunning = true;

    serverThread = std::thread([this]() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == 0) {
            std::cerr << "Socket failed\n";
            return;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(serverPort);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed\n";
            return;
        }

        if (listen(server_fd, 1) < 0) {
            std::cerr << "Listen failed\n";
            return;
        }

        std::cout << "[SocketServer] Waiting for GUI connection on port " << serverPort << "...\n";

        socklen_t addrlen = sizeof(address);
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            return;
        }

        std::cout << "[SocketServer] GUI connected!\n";

        while (serverRunning) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        close(client_fd);
        close(server_fd);
    });
}

void SocketServer::stop() {
    serverRunning = false;
    if (serverThread.joinable()) serverThread.join();
    if (client_fd != -1) close(client_fd);
    if (server_fd != -1) close(server_fd);
}

void SocketServer::sendMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);
    if (client_fd != -1) {
        std::string message = msg + "\n";
        send(client_fd, message.c_str(), message.size(), 0);
    }
}
