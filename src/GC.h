/*
 * GC.h
 * 
 * Hlavickovy soubor knihovny GC.
 * 
 */
#ifndef GC_H
#define GC_H

#include "Structs.h"

void* gcMalloc(size_t size);
void* gcRealloc(void* ptr, size_t size);
int gcFree(void* ptr);
void gcFreeAll();
void gcPrintState();
#endif
