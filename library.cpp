#include "library.h"

#include <iostream>
#include <ostream>
#include <stdio.h>

#include "Memory_Tracker.h"


using namespace std;


void* operator new(size_t size, const char* file, int line) {
    cout << "new operator overloading" << endl;
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}

void operator delete(void* p) noexcept {
    cout << "delete overloading" << endl;
    if (!p) return;
    MemoryTracker::getInstance().registerDeallocation(p);
    free(p);
}


void* operator new[](size_t size, const char* file, int line) {
    cout << "new[] operator overloading" << endl;
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}

void operator delete[](void* p) noexcept {
    cout << "delete[] overloading" << endl;
    if (!p) return;
    MemoryTracker::getInstance().registerDeallocation(p);
    free(p);
}

