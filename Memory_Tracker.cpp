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
}

void MemoryTracker::reportLeaks() {
    for (auto& [ptr, info] : allocations) {
        std::cout << "LEAK: " << info.size << " bytes at " << ptr
                  << " (file " << info.file << ", line " << info.line << ")\n";
    }
}



void MemoryTracker::registerDeallocation(void* ptr) {
    allocations.erase(ptr);
}


