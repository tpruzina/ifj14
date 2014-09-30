#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "GC.h"
#include "Structs.h"
#include "Log.h"

int addNewPtr(struct gc* gc, void* ptr){
	if(gc == NULL){
		// v pripade, ze gc nebyl jeste alocovan
		gc = (struct gc*) malloc(sizeof(struct gc));
		gc->list = NULL;
		gc->listLength = 0;
	}

	// vytvoreni noveho zaznamu v seznamu
	struct gcItem* newone = (struct gcItem*) malloc(sizeof(struct gcItem));
	if(newone != NULL) {
		newone->ptr = ptr;
		newone->next = gc->list;
		gc->list = newone;
		gc->listLength++;
		return True;
	}

	return False;
}

void* gcMalloc(struct mainAll** ma, size_t size){
	void* ptr = malloc(size);
	if(!addNewPtr((*ma)->gc, ptr)){
		return NULL;	
	}

	// kontrolni vypis
	char txt[100];
	sprintf(txt, "malloc\tOK (%p)", ptr);
	Log(txt, DEBUG, GC);

	return ptr;
}
/**
 * Realokuje pocet prirazenych prvku strukture.
 */
void* gcRealloc(struct mainAll** ma, void* ptr, size_t size){
	struct gcItem* item = (*ma)->gc->list;

	while(item != NULL){
		if(item->ptr == ptr){
			// nalezeni prislusneho odkazu
			// realokace a navraceni realokovaneho prvku
			item->ptr = realloc(ptr, size);
			char txt[100];
			sprintf(txt, "realloc\tOK (%p)", ptr);
			Log(txt, DEBUG, GC);
			return item->ptr;
		}
		item = item->next;	
	}
		
	return NULL;
}

int gcFree(struct mainAll** ma, void* ptr){
	// bitem a item slouzi pro provazovani ruseneho prvku
	struct gcItem* bitem;
	struct gcItem* item = (*ma)->gc->list;

	// pro jistotu
	if(item == NULL){
		Log("item == null\n", DEBUG, GC);
		return False;
	}

	// hledani prvku
	while(item->next != NULL){	
		if(item->next->ptr == ptr){
			bitem = item->next;
			item->next = bitem->next;
			
			free(bitem->ptr);
			free(bitem);
			(*ma)->gc->listLength--;
			Log("item freed", DEBUG, GC);
			return True;
		}
	
		item = item->next;
	}
	
	return False;
}

void gcPrintState(struct mainAll** ma){
	printf("Garbare collector state:\n");
	printf("\tReserved: %d\n", (*ma)->gc->listLength);

	struct gcItem* item = (*ma)->gc->list;
	int i = 0;
	while(item != NULL){
		printf("\t%d, %p\n", i++, item->ptr);
		item = item->next;		
	}
	printf("end\n");
}

void gcFreeAll(struct mainAll** ma){
	struct gcItem* item = (*ma)->gc->list;

	int i;
	while(item != NULL){
		// posun na dalsi item
		(*ma)->gc->list = item->next;

		// uvolneni alokovaneho prvku
		//Log("free item start", WARNING, GC);
		free(item->ptr);
		//Log("free item->ptr", WARNING, GC);
		free(item);
		//Log("free item", WARNING, GC);

		// posun rezervovanych	
		(*ma)->gc->listLength--;
		i++;
		// nastaveni na dalsi bod
		item = (*ma)->gc->list;
	}
	
	if((*ma)->gc->listLength == 0)
		Log("free_all:\tall free done", DEBUG, GC);
	else
		Log("free_add:\tmemory leak", DEBUG, GC);
}
