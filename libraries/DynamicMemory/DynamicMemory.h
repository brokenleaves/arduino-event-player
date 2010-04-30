#ifndef DynamicMemory_h
#define DynamicMemory_h

#include "WProgram.h"

void *operator new(size_t size);

void operator delete(void *ptr);

void *operator new[](size_t size);

void operator delete[](void *ptr);

#endif


