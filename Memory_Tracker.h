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

    // Singleton
    MemoryTracker()= default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;


public:
    static MemoryTracker& getInstance();

    void registerAllocation(void* ptr, size_t size, const std::string& file, int line);

    void registerDeallocation(void* ptr);

    void reportLeaks();

    // destructor: imprime fugas automáticamente al final
    ~MemoryTracker();

};



#endif //LIBRERIA_INSTRUMETALIZACION_MEMORY_TRACKER_H