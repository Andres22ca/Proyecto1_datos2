//
// Created by andres on 26/8/25.
// Este main es solo para pruebas, no tiene que estar en la version final ni mucho menos en la GUI


#include "Memory_Tracker.h"
#include <iostream>
#define new new (__FILE__, __LINE__)
#include "library.h"



int main() {
    auto& tracker = MemoryTracker::getInstance(); // inicia servidor

    int* a = new int;
    int* b = new int[5];

    delete a;
    // b no se borra intencionalmente para generar leak

    return 0;
}


