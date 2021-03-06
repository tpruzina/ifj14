/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativního jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antonín    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juřík Martin     <xjurik08@stud.fit.vutbr.cz           ****
****               David Petr       <xdavid15@stud.fit.vutbr.cz>          ****
****                                                                      ****
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "gc.h"
#include "structs.h"
#include "log.h"

struct mainAll global;

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

void* gcMalloc(size_t size){
	void* ptr = malloc(size);
	//void *ptr = calloc(sizeof(char), size);
	
	if(!addNewPtr(global.gc, ptr)){
		return NULL;	
	}

	// kontrolni vypis
	/*
	char txt[100];
	sprintf(txt, "malloc\tOK (%p)", ptr);
	Log(txt, DEBUG, GC);
	*/
	return ptr;
}
/**
 * Realokuje pocet prirazenych prvku strukture.
 */
void* gcRealloc(void* ptr, size_t size){
	struct gcItem* item = global.gc->list;

	while(item != NULL){
		if(item->ptr == ptr){
			// nalezeni prislusneho odkazu
			// realokace a navraceni realokovaneho prvku
			item->ptr = realloc(ptr, size);
			/*
			char txt[100];
			sprintf(txt, "realloc\tOK (%p)", ptr);
			Log(txt, DEBUG, GC);
			*/
			return item->ptr;
		}
		item = item->next;	
	}
		
	return NULL;
}

int gcFree(void* ptr){
	// bitem a item slouzi pro provazovani ruseneho prvku
	struct gcItem* bitem;
	struct gcItem* item = global.gc->list;

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
			global.gc->listLength--;
			
			return True;
		}
	
		item = item->next;
	}
	
	return False;
}

void gcFreeAll(){
	struct gcItem* item = global.gc->list;

	int i = 0;
	while(item != NULL){
		// posun na dalsi item
		global.gc->list = item->next;

		// uvolneni alokovaneho prvku
		free(item->ptr);
		free(item);
		
		// posun rezervovanych	
		global.gc->listLength--;
		i++;
		// nastaveni na dalsi bod
		item = global.gc->list;
	}
	
	if(global.gc->listLength == 0)
		Log("free_all:\tall free done", DEBUG, GC);
	else
		Log("free_add:\tmemory leak", DEBUG, GC);
}
