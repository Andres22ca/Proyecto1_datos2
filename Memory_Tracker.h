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
    MemoryTracker()= default;
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;


public:
    static MemoryTracker& getInstance();

    void registerAllocation(void* ptr, size_t size, const std::string& file, int line);

    void registerDeallocation(void* ptr);

    void reportLeaks();


};













#endif //LIBRERIA_INSTRUMETALIZACION_MEMORY_TRACKER_H