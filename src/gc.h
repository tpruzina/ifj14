/*
 * GC.h
 * 
 * Hlavickovy soubor knihovny GC.
 * 
 * 
 */
#ifndef GC_H
#define GC_H

#include "Structs.h"

void *gc_malloc(struct main_all **ma, size_t size);
void *gc_realloc(struct main_all **ma, void *ptr, size_t size);
int gc_free(struct main_all **ma, void *ptr);
void gc_free_all(struct main_all **ma);
void gc_print_state(struct main_all **ma);
#endif
