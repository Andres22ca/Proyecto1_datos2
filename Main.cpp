//
// Created by andres on 26/8/25.
// Este main es solo para pruebas, no tiene que estar en la version final ni mucho menos en la GUI


#include "Memory_Tracker.h"
#include <iostream>
#define new new (__FILE__, __LINE__)
#include "library.h"

int main() {
    MemoryTracker& tracker = MemoryTracker::getInstance();

    int* a = new int;        // registrará automáticamente
    int* arr = new int[5];   // registrará automáticamente

    delete a;                // liberación registrada
    // dejar arr sin delete para probar la fuga

    tracker.reportLeaks();   // debería mostrar arr como fuga

    return 0;
}

