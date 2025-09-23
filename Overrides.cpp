// Overrides.cpp
// Sobrecargas globales de new/delete para el profiler

#include "Memory_Tracker.h"
#include <cstdlib>     // malloc/free
#include <new>         // std::bad_alloc
#include <cstddef>     // std::size_t

// -----------------------------
// 1) NEW/DELETE "NORMALES"
// -----------------------------

void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(ptr, size, "unknown", 0);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}

// sized delete (algunos compiladores la llaman)
void operator delete(void* ptr, std::size_t) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}

// Arrays
void* operator new[](std::size_t size) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(ptr, size, "unknown", 0);
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}

void operator delete[](void* ptr, std::size_t) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}

// -----------------------------
// 2) NEW/DELETE con (file, line) — usado por MP_NEW
// -----------------------------

void* operator new(std::size_t size, const char* file, int line) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(ptr, size, file, line);
    return ptr;
}

// delete “pareja” para cuando el ctor lanza
void operator delete(void* ptr, const char* /*file*/, int /*line*/) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}

void* operator new[](std::size_t size, const char* file, int line) {
    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(ptr, size, file, line);
    return ptr;
}

void operator delete[](void* ptr, const char* /*file*/, int /*line*/) noexcept {
    if (ptr) {
        MemoryTracker::getInstance().registerDeallocation(ptr);
        std::free(ptr);
    }
}
