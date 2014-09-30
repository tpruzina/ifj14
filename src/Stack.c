#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "GC.h"

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

int stackPush(struct stack* stack, void* val){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return False;
	}
	
	// vytvoreni nove polozky, naplneni daty a provazani s vrcholem
	struct stackItem* newitem = (struct stackItem*) gcMalloc(sizeof(struct stackItem));
	if(newitem == NULL){
		Log("Allocation error", ERROR, STACK);
		global.errno = intern;
		return False;	
	}
	newitem->Value = val;
	newitem->Next = stack->Top;
	stack->Top = newitem;
	stack->Length++;
	return True;
}

void* stackPop(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
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
		Log("Stack is empty...", WARNING, STACK);
		return NULL;	
	}
}

void* stackTop(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return NULL;
	}
	
	if(stack->Top != NULL){
		// vraceni hodnoty navrcholu
		return stack->Top->Value;
	}
	else {
		Log("Stack is empty...", WARNING, STACK);
		return NULL;
	}	
}

int stackEmpty(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		global.errno = intern;
		return False;
	}

	return (stack->Length == 0);
}


