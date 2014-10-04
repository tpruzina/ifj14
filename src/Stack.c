#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "GC.h"

/**
 * Vytvori novou strukturu zasobniku. S vynulovanym vrcholem a nulovou delkou.
 */
struct stack* makeNewStack(){
	// alokace nove polozky
	struct stack* stc = (struct stack*)gcMalloc(sizeof(struct stack));
	if(stc == NULL){
		Log("Allocation error", ERROR, STACK);
		global.errno = intern;
		return NULL;
	}	
	
	// Nastaveni hodnot
	stc->Top = NULL;
	stc->Length = 0;	
	return stc;
}

/**
 * Vlozi na vrchol zasobniku novy uzel.
 */
int stackPush(struct stack* stack, void* val){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackPush: Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return False;
	}
	
	// vytvoreni nove polozky, naplneni daty a provazani s vrcholem
	struct stackItem* newitem = (struct stackItem*) gcMalloc(sizeof(struct stackItem));
	if(newitem == NULL){
		Log("stackPush: Allocation error", ERROR, STACK);
		global.errno = intern;
		return False;	
	}
	newitem->Value = val;
	newitem->Next = stack->Top;
	stack->Top = newitem;
	stack->Length++;
	return True;
}

/**
 * Odstrani a vrati vrchol zasobniku.
 */
void* stackPop(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackPop: Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return NULL;
	}
	
	if(stack->Top != NULL){
		// provazani nasledujiciho na misto prvniho
		struct stackItem* item = stack->Top;
		stack->Top = stack->Top->Next;
		// kopie hodnoty navrcholu
		void* val = item->Value;
		// uvolneni prvku navrcholu
		gcFree(item);
		// zmenseni velikosti zasobniku
		stack->Length--;
		// vraceni hodnoty
		return val;
	}
	else {
		Log("stackPop: Stack is empty...", WARNING, STACK);
		return NULL;	
	}
}
/**
 * Vrati vrchol zasobniku, bez toho aby jej odstranil.
 */
void* stackTop(struct stack* stack) {
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackTop: Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return NULL;
	}
	
	if(stack->Top != NULL){
		// vraceni hodnoty navrcholu
		return stack->Top->Value;
	}
	else {
		Log("stackTop: Stack is empty...", WARNING, STACK);
		return NULL;
	}	
}
/**
 * Kontrola jen na prazdnost zasobniku.
 */
int stackEmpty(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackEmpty: Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return False;
	}

	return (stack->Length == 0);
}

/**
 * Vytvari novou instanci fronty
 * --------------------
 */
struct queue* makeNewQueue(){
	struct queue* que = (struct queue*)gcMalloc(sizeof(struct queue));
	if(que == NULL){
		Log("queue: Allocation error", ERROR, STACK);
		global.errno = intern;
		return NULL;
	}

	que->start = NULL;
	que->end = NULL;
	return que;
}
/**
 * Vklada do fronty nakonec prvek
 * --------------------
 * que: fronta, do ktere ma byt prvek vlozen
 * value: hodnota, ktera ma byt vlozena
 */
int queuePush(struct queue* que, void* value){
	if(que == NULL)
		return False;
		
	struct queueItem* item = (struct queueItem*)gcMalloc(sizeof(struct queueItem));
	if(item == NULL)
	  	return False;
	  	
	item->value = value;
	item->next = NULL;
	
	if(que->start == NULL)
		que->start = item;
	else 
		que->end->next = item;
	que->end = item;
	
	return True;
}
/**
 * Odstrani z vrcholu prvek a vrati jeho hodnotu
 * --------------------
 * que: fronta na odstraneni prvku
 */
void* queuePop(struct queue* que){
	if(que == NULL)
		return NULL;
		
	struct queueItem* item = que->start;
	if(item == que->end)
		que->end = NULL;
	
	que->start = que->start->next;
	void* ptr = item->value;
	gcFree(item);
	return ptr;
}
/**
 * Vraci hodnotu na vrcholu fronty, bez toho aby ho odstranila
 * --------------------
 * que: fronta
 */
void* queueTop(struct queue* que){
	if(que == NULL)	
		return NULL;
	
	return que->start->value;	
}
/**
 * Vraci True v pripade, ze se prvni i posledni prvek rovnaji a jsou NULL
 * --------------------
 * que: fronta pro zjisteni
 */
int queueEmpty(struct queue* que){
	if(que == NULL)
		return -1;
	
	if(que->start == que->end && que->start == NULL)
		return True;
	else 
		return False;
}
