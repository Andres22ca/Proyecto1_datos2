//
// Created by andres on 26/8/25.
// Este main es solo para pruebas, no tiene que estar en la version final ni mucho menos en la GUI


#include "Memory_Tracker.h"
#include <iostream>

int main() {
    // referencia al singleton
    MemoryTracker& tracker = MemoryTracker::getInstance();

    // simular una asignación (normalmente esto lo haría operator new)
    int* x = (int*)malloc(sizeof(int));
    tracker.registerAllocation(x, sizeof(int), "main.cpp", __LINE__);

    // simular otra asignación
    int* y = (int*)malloc(10 * sizeof(int));
    tracker.registerAllocation(y, 10 * sizeof(int), "main.cpp", __LINE__);

    // liberar una de las asignaciones
    free(x);
    tracker.registerDeallocation(x);

    // reporte de fugas al final
    tracker.reportLeaks();

    return 0;
}
