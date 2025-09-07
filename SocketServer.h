//
// Created by andres on 7/9/25.
//

#ifndef LIBRERIA_INSTRUMETALIZACION_SOCKETSERVER_H
#define LIBRERIA_INSTRUMETALIZACION_SOCKETSERVER_H


#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class SocketServer {
private:
    int server_fd = -1;
    int client_fd = -1;
    std::thread serverThread;
    std::mutex mtx;
    bool serverRunning = false;
    int serverPort;

public:
    SocketServer(int port = 54000);
    ~SocketServer();

    void start();
    void stop();
    void sendMessage(const std::string& msg);
};


#endif //LIBRERIA_INSTRUMETALIZACION_SOCKETSERVER_H