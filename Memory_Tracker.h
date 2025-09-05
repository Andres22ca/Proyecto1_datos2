//
// Created by andres on 24/8/25.
//

#ifndef LIBRERIA_INSTRUMETALIZACION_MEMORY_TRACKER_H
#define LIBRERIA_INSTRUMETALIZACION_MEMORY_TRACKER_H

#include "library.h"
#include <iostream>
#include <string>
#include<chrono>
#include<unordered_map>
#include<cstddef>

#include <thread>
#include <mutex>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>



//donde se guarda la info
struct  AllocationInfo {
    void* address; //Direccion
    size_t size;//tamano en bits
    std ::string file; //archivo
    int line; // Liena del archivvo
    long long timestamp; //cuando paso

};

//Administracion y reporte
class MemoryTracker {
private:
    std::unordered_map<void*, AllocationInfo> allocations;

    // métricas generales
    size_t totalAllocations = 0;       // total de llamadas a new/new[]
    size_t activeAllocations = 0;      // asignaciones activas
    size_t currentMemoryUsage = 0;     // memoria actual en bytes
    size_t peakMemoryUsage = 0;        // memoria máxima usada
    size_t leakedMemory = 0;           // total fugado

    // Para el servidor TCP
    int server_fd = -1;           // Socket del servidor
    int client_fd = -1;           // Socket del cliente (GUI)
    std::thread serverThread;     // Hilo para el servidor
    std::mutex mtx;               // Para proteger envíos concurrentes
    bool serverRunning = false;   // Control del hilo del servidor
    int serverPort = 54000;       // Puerto por defecto


    // Singleton
    MemoryTracker()= default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;


public:
    static MemoryTracker& getInstance();

    void registerAllocation(void* ptr, size_t size, const std::string& file, int line);

    void registerDeallocation(void* ptr);

    void reportLeaks();


    std::string getMetricsJSON() {
        std::ostringstream oss;
        oss << "{";
        oss << "\"totalAllocations\":" << totalAllocations << ",";
        oss << "\"activeAllocations\":" << activeAllocations << ",";
        oss << "\"currentMemoryUsage\":" << currentMemoryUsage << ",";
        oss << "\"peakMemoryUsage\":" << peakMemoryUsage;
        oss << "}";
        return oss.str();
    }


    void sendUpdateToGUI() {
        std::lock_guard<std::mutex> lock(mtx);
        if (client_fd != -1) {
            std::string json = getMetricsJSON();
            json += "\n"; // separador para que la GUI lea línea por línea
            send(client_fd, json.c_str(), json.size(), 0);
        }
    }


    void startServer(int port = 54000) {
        serverPort = port;
        serverRunning = true;

        serverThread = std::thread([this]() {
            server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd == 0) {
                std::cerr << "Socket failed\n";
                return;
            }

            sockaddr_in address;
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

            std::cout << "[MemoryTracker] Waiting for GUI connection on port " << serverPort << "...\n";

            socklen_t addrlen = sizeof(address);
            client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
            if (client_fd < 0) {
                std::cerr << "Accept failed\n";
                return;
            }

            std::cout << "[MemoryTracker] GUI connected!\n";

            while (serverRunning) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            close(client_fd);
            close(server_fd);
        });
    }


    // destructor: imprime fugas automáticamente al final
    ~MemoryTracker();

};



#endif //LIBRERIA_INSTRUMETALIZACION_MEMORY_TRACKER_H