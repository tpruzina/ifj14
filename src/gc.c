#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "GC.h"
#include "Constants.h"
#include "Structs.h"
#include "Log.h"

int add_new_ptr(struct gc* gc, void* ptr){
	if(gc == NULL){
		// v pripade, ze gc nebyl jeste alocovan
		gc = (struct gc*) malloc(sizeof(struct gc));
		gc->list = NULL;
		gc->list_length = 0;
	}

	// vytvoreni noveho zaznamu v seznamu
	struct gc_item* newone = (struct gc_item*) malloc(sizeof(struct gc_item));
	if(newone != NULL) {
		newone->ptr = ptr;
		newone->next = gc->list;
		gc->list = newone;
		gc->list_length++;
		return True;
	}

	return False;
}

void* gc_malloc(struct main_all** ma, size_t size){
	void* ptr = malloc(size);
	if(!add_new_ptr((*ma)->gc, ptr)){
		return NULL;	
	}

	// kontrolni vypis
	char txt[100];
	sprintf(txt, "gc:malloc:OK (%p)", ptr);
	LogDebug(txt);

	
	return ptr;
}

void* gc_realloc(struct main_all** ma, void* ptr, size_t size){
	struct gc_item* item = (*ma)->gc->list;

	while(item != NULL){
		if(item->ptr == ptr){
			// nalezeni prislusneho odkazu
			// realokace a navraceni realokovaneho prvku
			item->ptr = realloc(ptr, size);
			return item->ptr;
		}	
	}
		
	return NULL;
}

int gc_free(struct main_all** ma, void* ptr){
	// bitem a item slouzi pro provazovani ruseneho prvku
	struct gc_item* bitem = malloc(sizeof(struct gc_item));
	struct gc_item* item = (*ma)->gc->list;

	// pro jistotu
	if(item == NULL){
		LogDebug("gc: item == null\n");
		return False;
	}

	// hledani prvku
	while(item->ptr != ptr && item->next != NULL){			
		bitem = item;
		item = item->next;
	}

	if(item->ptr == ptr){
		LogDebug("gc:free:item found");
		// provazani
		bitem->next = item->next;
		// uvolneni hledaneho prvku
		free(item->ptr);
		free(item);
		// dekrementace delky seznamu
		(*ma)->gc->list_length--;

		// v pripade nalezeni prvku
		free(bitem);
		LogDebug("gc:free:item is free");
		return True;
	}

	// v pripade nenalezeni taky smazat
	free(bitem);
	return False;
}

void gc_print_state(struct main_all** ma){
	printf("Garbare collector state:\n");
	printf("\tReserved: %d\n", (*ma)->gc->list_length);

	struct gc_item* item = (*ma)->gc->list;
	int i = 0;
	while(item != NULL){
		printf("\t%d, %p\n", i++, item->ptr);
		item = item->next;		
	}
	printf("end\n");
}

void gc_free_all(struct main_all** ma){
	struct gc_item* item = (*ma)->gc->list;

	int i;
	while(item != NULL){
		// posun na dalsi item
		(*ma)->gc->list = item->next;

		// uvolneni alokovaneho prvku
		free(item->ptr);
		free(item);

		// posun rezervovanych	
		(*ma)->gc->list_length--;
		i++;
		// nastaveni na dalsi bod
		item = (*ma)->gc->list;
	}

	LogDebug("gc:free_all: all free done");
}
