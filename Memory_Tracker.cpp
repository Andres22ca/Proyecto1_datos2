//
// Created by andres on 24/8/25.
//

#include "Memory_Tracker.h"

MemoryTracker& MemoryTracker::getInstance() {
    static MemoryTracker instance;
    return instance;
}


void MemoryTracker::registerAllocation(void* ptr, size_t size, const std::string& file, int line) {
    AllocationInfo info{ptr, size, file, line,
        std::chrono::system_clock::now().time_since_epoch().count()};
    allocations[ptr] = info;

    // actualizar métricas
    totalAllocations++;
    activeAllocations++;
    currentMemoryUsage += size;
    if (currentMemoryUsage > peakMemoryUsage) {
        peakMemoryUsage = currentMemoryUsage;
    }

}

void MemoryTracker::registerDeallocation(void* ptr) {
    auto it = allocations.find(ptr);
    if (it != allocations.end()) {
        currentMemoryUsage -= it->second.size;
        activeAllocations--;
        allocations.erase(it);
    }
}

void MemoryTracker::reportLeaks() {
    leakedMemory = 0;

    std::cout << "\n--- Memory Report ---\n";
    std::cout << "Total allocations: " << totalAllocations << "\n";
    std::cout << "Active allocations: " << activeAllocations << "\n";
    std::cout << "Peak memory usage: " << peakMemoryUsage << " bytes\n";

    if (!allocations.empty()) {
        std::cout << "Leaks detected:\n";
        size_t biggestLeak = 0;
        std::string biggestLeakFile;

        for (auto& [ptr, info] : allocations) {
            std::cout << "LEAK: " << info.size << " bytes at " << ptr
                      << " (file " << info.file << ", line " << info.line << ")\n";
            leakedMemory += info.size;

            if (info.size > biggestLeak) {
                biggestLeak = info.size;
                biggestLeakFile = info.file;
            }
        }
        std::cout << "Total leaked memory: " << leakedMemory << " bytes\n";
        std::cout << "Biggest leak: " << biggestLeak << " bytes (in " << biggestLeakFile << ")\n";
        double leakRate = (totalAllocations > 0) ? (double)activeAllocations / totalAllocations * 100 : 0.0;
        std::cout << "Leak rate: " << leakRate << "% of allocations\n";
    } else {
        std::cout << "No leaks detected.\n";
    }

    std::cout << "---------------------\n";
}

MemoryTracker::~MemoryTracker() {
    if (!allocations.empty()) {
        std::cout << "\n[!] Program ended with leaks:\n";
        reportLeaks();
    }
}





