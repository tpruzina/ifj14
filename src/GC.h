/*
 * GC.h
 * 
 * Hlavickovy soubor knihovny GC.
 * 
 */
#ifndef GC_H
#define GC_H

#include "Structs.h"

void* gcMalloc(struct mainAll** ma, size_t size);
void* gcRealloc(struct mainAll** ma, void* ptr, size_t size);
int gcFree(struct mainAll** ma, void* ptr);
void gcFreeAll(struct mainAll** ma);
void gcPrintState(struct mainAll** ma);
#endif
