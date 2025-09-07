#include "library.h"
#include <iostream>
#include <cstdlib>
#include "Memory_Tracker.h"

using namespace std;

void* operator new(size_t size, const char* file, int line) {
    cout << "new operator overloading\n";
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}

void operator delete(void* p) noexcept {
    if (!p) return;
    if (MemoryTracker::getInstance().isTracked(p)) {
        cout << "delete overloading\n";
        MemoryTracker::getInstance().registerDeallocation(p);
        free(p);
    }
}

void* operator new[](size_t size, const char* file, int line) {
    cout << "new[] operator overloading\n";
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}

void operator delete[](void* p) noexcept {
    if (!p) return;
    if (MemoryTracker::getInstance().isTracked(p)) {
        cout << "delete[] overloading\n";
        MemoryTracker::getInstance().registerDeallocation(p);
        free(p);
    }
}
