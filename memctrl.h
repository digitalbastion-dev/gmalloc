#ifndef MEMCTRL_H
#define MEMCTRL_H
#include <stddef.h>

void* gmalloc(size_t size);
void* grealloc(void* ptr, size_t size);

void gfree(void* ptr);


#endif