//
// Created by andres on 24/8/25.
//

#pragma once
#include "SocketServer.h"
#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <cstddef>
#include <sstream>
#include <mutex>

struct AllocationInfo {
    void* address;
    size_t size;
    std::string file;
    int line;
    long long timestamp;
};

class MemoryTracker {
private:
    std::unordered_map<void*, AllocationInfo> allocations;
    SocketServer socketServer;

    std::atomic<size_t> totalAllocations{0};    // atómicos: se actualizan muy seguido
    std::atomic<size_t> activeAllocations{0};
    std::atomic<size_t> currentMemoryUsage{0};
    std::atomic<size_t> peakMemoryUsage{0};
    std::atomic<size_t> leakedMemory{0};

    mutable std::mutex mtx_;

    static thread_local bool inTracker_;

    // (NUEVO) Guard de ámbito para desactivar tracking interno
    struct InternalGuard {
        InternalGuard()  { MemoryTracker::inTracker_ = true; }
        ~InternalGuard() { MemoryTracker::inTracker_ = false; }
    };


    MemoryTracker();
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;

public:
    static MemoryTracker& getInstance();

    void registerAllocation(void* ptr, size_t size, const std::string& file, int line);
    void registerDeallocation(void* ptr);
    void reportLeaks();
    std::string getMetricsJSON();
    void sendUpdateToGUI();
    bool isTracked(void* ptr);
    //métodos para enviar eventos puntuales
    std::string getAllocEventJSON(const AllocationInfo& info);
    std::string getFreeEventJSON(void* ptr);



    ~MemoryTracker();
};
