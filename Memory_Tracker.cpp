//
// Created by andres on 24/8/25.
//

#include "Memory_Tracker.h"

MemoryTracker::MemoryTracker() : socketServer(54000) {
    socketServer.start();
}

MemoryTracker& MemoryTracker::getInstance() {
    static MemoryTracker instance;
    return instance;
}

void MemoryTracker::registerAllocation(void* ptr, size_t size, const std::string& file, int line) {
    AllocationInfo info{ptr, size, file, line, std::chrono::system_clock::now().time_since_epoch().count()};
    allocations[ptr] = info;

    totalAllocations++;
    activeAllocations++;
    currentMemoryUsage += size;
    if (currentMemoryUsage > peakMemoryUsage) peakMemoryUsage = currentMemoryUsage;

    sendUpdateToGUI();
}

void MemoryTracker::registerDeallocation(void* ptr) {
    auto it = allocations.find(ptr);
    if (it != allocations.end()) {
        currentMemoryUsage -= it->second.size;
        activeAllocations--;
        allocations.erase(it);
    }
    sendUpdateToGUI();
}

bool MemoryTracker::isTracked(void* ptr) {
    return allocations.find(ptr) != allocations.end();
}

void MemoryTracker::reportLeaks() {
    leakedMemory = 0;
    std::cout << "\n--- Memory Report ---\n";
    std::cout << "Total allocations: " << totalAllocations << "\n";
    std::cout << "Active allocations: " << activeAllocations << "\n";
    std::cout << "Peak memory usage: " << peakMemoryUsage << " bytes\n";

    if (!allocations.empty()) {
        std::cout << "Leaks detected:\n";
        for (auto& [ptr, info] : allocations) {
            std::cout << "LEAK: " << info.size << " bytes at " << ptr
                      << " (file " << info.file << ", line " << info.line << ")\n";
            leakedMemory += info.size;
        }
        std::cout << "Total leaked memory: " << leakedMemory << " bytes\n";
    } else {
        std::cout << "No leaks detected.\n";
    }

    std::cout << "---------------------\n";
}

std::string MemoryTracker::getMetricsJSON() {
    std::ostringstream oss;
    oss << "{";
    oss << "\"totalAllocations\":" << totalAllocations << ",";
    oss << "\"activeAllocations\":" << activeAllocations << ",";
    oss << "\"currentMemoryUsage\":" << currentMemoryUsage << ",";
    oss << "\"peakMemoryUsage\":" << peakMemoryUsage;
    oss << "}";
    return oss.str();
}

void MemoryTracker::sendUpdateToGUI() {
    socketServer.sendMessage(getMetricsJSON());
}

MemoryTracker::~MemoryTracker() {
    if (!allocations.empty()) {
        std::cout << "\n[!] Program ended with leaks:\n";
        reportLeaks();
    }
}







