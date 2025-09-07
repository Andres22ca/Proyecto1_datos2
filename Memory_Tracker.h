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

    size_t totalAllocations = 0;
    size_t activeAllocations = 0;
    size_t currentMemoryUsage = 0;
    size_t peakMemoryUsage = 0;
    size_t leakedMemory = 0;

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

    ~MemoryTracker();
};
