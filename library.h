#ifndef LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
#define LIBRERIA_INSTRUMETALIZACION_LIBRARY_H

#include <cstddef>
#include <new>

// Firmas estándar (asegurar compatibilidad con librerías)
void* operator new(std::size_t size);
void operator delete(void* ptr) noexcept;
void* operator new[](std::size_t size);
void operator delete[](void* ptr) noexcept;

// Nuestras firmas con file/line (usadas por el macro)
void* operator new(std::size_t size, const char* file, int line);
void operator delete(void* ptr, const char* file, int line) noexcept; // opcional
void* operator new[](std::size_t size, const char* file, int line);
void operator delete[](void* ptr, const char* file, int line) noexcept; // opcional

#endif // LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
