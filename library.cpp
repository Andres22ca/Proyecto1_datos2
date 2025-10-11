#include "library.h"
#include <iostream>
#include <cstdlib>
#include "Memory_Tracker.h"

using namespace std;
// Sobrecarga del operador new: registra una nueva asignación de memoria con información del archivo y línea
void* operator new(size_t size, const char* file, int line) {
    cout << "new operator overloading\n";
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}
// Sobrecarga del operador delete: registra la liberación de memoria si el puntero fue rastreado
void operator delete(void* p) noexcept {
    if (!p) return;
    if (MemoryTracker::getInstance().isTracked(p)) {
        cout << "delete overloading\n";
        MemoryTracker::getInstance().registerDeallocation(p);
        free(p);
    }
}
// Sobrecarga del operador new[]: registra una nueva asignación de un arreglo con información del archivo y línea
void* operator new[](size_t size, const char* file, int line) {
    cout << "new[] operator overloading\n";
    void* p = malloc(size);
    if (!p) throw std::bad_alloc();
    MemoryTracker::getInstance().registerAllocation(p, size, file, line);
    return p;
}
// Sobrecarga del operador delete[]: registra la liberación de un arreglo si el puntero fue rastreado
void operator delete[](void* p) noexcept {
    if (!p) return;
    if (MemoryTracker::getInstance().isTracked(p)) {
        cout << "delete[] overloading\n";
        MemoryTracker::getInstance().registerDeallocation(p);
        free(p);
    }
}
