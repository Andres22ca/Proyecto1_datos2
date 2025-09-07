#include "Memory_Tracker.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "[Main] Starting Memory Tracker...\n";

    // Inicializa el tracker y el socket server
    MemoryTracker& tracker = MemoryTracker::getInstance();

    std::cout << "[Main] Memory Tracker is running. Waiting for GUI connections...\n";

    // Mantener el programa vivo para que el servidor siga corriendo
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
