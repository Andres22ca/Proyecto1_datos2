#ifndef LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
#define LIBRERIA_INSTRUMETALIZACION_LIBRARY_H
#include <cstddef>


void* operator new(size_t size);
void operator delete(void* ptr);

void* operator new[](size_t size);
void operator delete[](void* ptr);



#endif // LIBRERIA_INSTRUMETALIZACION_LIBRARY_H