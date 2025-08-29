#ifndef LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
#define LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
#include <cstddef>


void* operator new(size_t size, const char* file, int line);
void operator delete(void* ptr) noexcept;

void* operator new[](size_t size, const char* file, int line);
void operator delete[](void* ptr) noexcept;



#endif // LIBRERIA_INSTRUMETALIZACION_LIBRARY_H