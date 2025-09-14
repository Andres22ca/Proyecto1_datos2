//
// Created by andres on 24/8/25.
//

#include "Memory_Tracker.h"
#include <atomic>
#include <mutex>
#include <vector>


thread_local bool MemoryTracker::inTracker_ = false;

MemoryTracker::MemoryTracker() : socketServer(54000) {
    socketServer.start();
}

MemoryTracker& MemoryTracker::getInstance() {
    static MemoryTracker instance;
    return instance;
}

// Memory_Tracker.cpp
void MemoryTracker::registerAllocation(void* ptr, size_t size, const std::string& file, int line) {
    if (inTracker_) return;           // evita reentrada
    InternalGuard guard;           // desactiva tracking interno durante este método

    AllocationInfo info{ptr, size, file, line,
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    };

    {   // región crítica corta
        std::lock_guard<std::mutex> lock(mtx_);
        allocations[ptr] = info;
    }

    totalAllocations.fetch_add(1, std::memory_order_relaxed);
    activeAllocations.fetch_add(1, std::memory_order_relaxed);
    auto cur = currentMemoryUsage.fetch_add(size, std::memory_order_relaxed) + size;

    // actualizar pico sin lock (técnica CAS simple)
    size_t prevPeak = peakMemoryUsage.load(std::memory_order_relaxed);
    while (cur > prevPeak && !peakMemoryUsage.compare_exchange_weak(prevPeak, cur, std::memory_order_relaxed)) {}

    // Construye payloads FUERA del lock y envía
    socketServer.sendMessage(getAllocEventJSON(info));
    sendUpdateToGUI();
}

void MemoryTracker::registerDeallocation(void* ptr) {
    if (inTracker_) return;
    InternalGuard guard;

    size_t freed = 0;
    AllocationInfo removed{};
    bool wasTracked = false;

    {   // región crítica corta
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            freed = it->second.size;
            removed = it->second;
            allocations.erase(it);
            wasTracked = true;
        }
    }

    if (wasTracked) {
        activeAllocations.fetch_sub(1, std::memory_order_relaxed);
        currentMemoryUsage.fetch_sub(freed, std::memory_order_relaxed);
    }

    // Notifica evento (aún si no estaba trackeado, puedes mandar un 'free' igual)
    socketServer.sendMessage(getFreeEventJSON(ptr));
    sendUpdateToGUI();
}

bool MemoryTracker::isTracked(void* ptr) {
    std::lock_guard<std::mutex> lock(mtx_);
    return allocations.find(ptr) != allocations.end();
}



void MemoryTracker::reportLeaks() {
    // ---- 1) Snapshot bajo lock (región crítica corta) ----
    std::vector<AllocationInfo> leaks;
    leaks.reserve(allocations.size());

    {
        std::lock_guard<std::mutex> lock(mtx_);
        for (const auto& kv : allocations) {
            leaks.push_back(kv.second);
        }
    }

    // ---- 2) Agregaciones fuera del lock ----
    const size_t leakCount = leaks.size();
    size_t totalLeakedBytes = 0;

    // Leak más grande
    size_t largestLeakBytes = 0;
    const AllocationInfo* largestLeak = nullptr;

    // Agregación por archivo
    struct Agg { size_t count = 0; size_t bytes = 0; };
    std::unordered_map<std::string, Agg> byFile;

    for (const auto& info : leaks) {
        totalLeakedBytes += info.size;

        if (info.size > largestLeakBytes) {
            largestLeakBytes = info.size;
            largestLeak = &info;
        }

        auto& agg = byFile[info.file];
        agg.count += 1;
        agg.bytes += info.size;
    }

    // Archivo con mayor frecuencia de leaks (desempata por bytes)
    std::string worstFile;
    size_t worstFileCount = 0;
    size_t worstFileBytes = 0;
    for (const auto& kv : byFile) {
        const auto& file = kv.first;
        const auto& agg  = kv.second;
        if (agg.count > worstFileCount ||
            (agg.count == worstFileCount && agg.bytes > worstFileBytes)) {
            worstFile      = file;
            worstFileCount = agg.count;
            worstFileBytes = agg.bytes;
        }
    }

    // Tasa de leaks (por conteo) respecto al total de asignaciones
    const auto totalAllocs = totalAllocations.load(std::memory_order_relaxed);
    double leakRate = 0.0;
    if (totalAllocs > 0) {
        leakRate = static_cast<double>(leakCount) / static_cast<double>(totalAllocs);
    }

    // Actualiza métrica interna (atomic store)
    leakedMemory.store(totalLeakedBytes, std::memory_order_relaxed);

    // ---- 3) Imprime reporte humano (protegido contra reentrada) ----
    {
        InternalGuard guard; // evita recursión por iostream/strings
        std::cout << "\n--- Memory Report ---\n";
        std::cout << "Total allocations: " << totalAllocs << "\n";
        std::cout << "Active allocations: " << activeAllocations.load(std::memory_order_relaxed) << "\n";
        std::cout << "Peak memory usage: " << peakMemoryUsage.load(std::memory_order_relaxed) << " bytes\n";
        std::cout << "Leaks detected: " << leakCount << "\n";
        std::cout << "Total leaked memory: " << totalLeakedBytes << " bytes\n";

        if (largestLeak) {
            std::cout << "Largest leak: " << largestLeakBytes << " bytes at " << largestLeak->address
                      << " (file " << largestLeak->file << ", line " << largestLeak->line << ")\n";
        } else {
            std::cout << "Largest leak: n/a\n";
        }

        if (!worstFile.empty()) {
            std::cout << "File with most leaks: " << worstFile
                      << " (count=" << worstFileCount
                      << ", bytes=" << worstFileBytes << ")\n";
        } else {
            std::cout << "File with most leaks: n/a\n";
        }

        std::cout << "Leak rate (count/allocs): " << (leakRate * 100.0) << "%\n";

        if (!leaks.empty()) {
            std::cout << "\nLeaks detail:\n";
            for (const auto& info : leaks) {
                std::cout << "  LEAK " << info.size << " bytes at " << info.address
                          << " (file " << info.file << ", line " << info.line << ")\n";
            }
        } else {
            std::cout << "No leaks detected.\n";
        }
        std::cout << "---------------------\n";
    }

    // ---- 4) Envía resumen a la GUI (JSON, protegido contra reentrada) ----
    {
        InternalGuard guard; // evita recursión durante la construcción de JSON
        std::ostringstream oss;
        oss << R"({"type":"leaks_summary","totalAllocations":)"
            << totalAllocs
            << R"(,"activeAllocations":)"
            << activeAllocations.load(std::memory_order_relaxed)
            << R"(,"peakMemoryUsage":)"
            << peakMemoryUsage.load(std::memory_order_relaxed)
            << R"(,"leakCount":)"
            << leakCount
            << R"(,"totalLeakedBytes":)"
            << totalLeakedBytes
            << R"(,"leakRate":)"
            << leakRate
            << R"(,)";

        // Largest leak
        if (largestLeak) {
            oss << R"("largestLeak":{"address":")"
                << largestLeak->address
                << R"(","size":)"
                << largestLeak->size
                << R"(,"file":")"
                << largestLeak->file
                << R"(","line":)"
                << largestLeak->line
                << R"(},)";
        } else {
            oss << R"("largestLeak":null,)";
        }

        // Worst file
        if (!worstFile.empty()) {
            oss << R"("worstFile":{"file":")"
                << worstFile
                << R"(","count":)"
                << worstFileCount
                << R"(,"bytes":)"
                << worstFileBytes
                << R"(},)";
        } else {
            oss << R"("worstFile":null,)";
        }

        // Lista de leaks (para gráficas/tabla en GUI)
        oss << R"("leaks":[)";
        for (size_t i = 0; i < leaks.size(); ++i) {
            const auto& info = leaks[i];
            oss << R"({"address":")"
                << info.address
                << R"(","size":)"
                << info.size
                << R"(,"file":")"
                << info.file
                << R"(","line":)"
                << info.line
                << R"(,"timestamp":)"
                << info.timestamp
                << R"(})";
            if (i + 1 < leaks.size()) oss << R"(,)";
        }
        oss << R"(]})";

        socketServer.sendMessage(oss.str());
    }
}



std::string MemoryTracker::getMetricsJSON() {
    InternalGuard guard;  // evita reentrada durante la construcción del JSON

    const auto totalAllocs = totalAllocations.load(std::memory_order_relaxed);
    const auto activeAllocs = activeAllocations.load(std::memory_order_relaxed);
    const auto currentMem   = currentMemoryUsage.load(std::memory_order_relaxed);
    const auto peakMem      = peakMemoryUsage.load(std::memory_order_relaxed);

    std::ostringstream oss;
    oss << R"({"type":"metrics","totalAllocations":)"
        << totalAllocs
        << R"(,"activeAllocations":)"
        << activeAllocs
        << R"(,"currentMemoryUsage":)"
        << currentMem
        << R"(,"peakMemoryUsage":)"
        << peakMem
        << R"(})";
    return oss.str();
}


std::string MemoryTracker::getAllocEventJSON(const AllocationInfo& info) {
    InternalGuard guard;
    std::ostringstream oss;
    oss << R"({"type":"alloc","address":")"
        << info.address
        << R"(","size":)"
        << info.size
        << R"(,"file":")"
        << info.file
        << R"(","line":)"
        << info.line
        << R"(})";
    return oss.str();
}

std::string MemoryTracker::getFreeEventJSON(void* ptr) {
    InternalGuard guard;  // opcional pero recomendado

    // Si no necesitas timestamp, borra este bloque y su uso abajo.
    auto ts = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now().time_since_epoch()
              ).count();

    std::ostringstream oss;
    oss << R"({"type":"free","address":")"
        << ptr
        << R"(","timestamp":)"
        << ts
        << R"(})";
    return oss.str();
}



void MemoryTracker::sendUpdateToGUI() {
    socketServer.sendMessage(getMetricsJSON());
}

MemoryTracker::~MemoryTracker() {
    if (!allocations.empty()) {
        std::cout << "\n[!] Program ended with leaks:\n";
        reportLeaks();
    }
}







