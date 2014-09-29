#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "GC.h"

struct stack* makeNewStack(struct mainAll** ma){
	// kontrola na hlavni strukturu
	if((*ma) == NULL){
		Log("Main_all = NULL", ERROR, STACK);		
		(*ma)->errno = intern;
		return NULL;
	}
		
	// alokace nove polozky
	struct stack* stc = (struct stack*)gcMalloc(ma, sizeof(struct stack));
	if(stc == NULL){
		Log("Allocation error", ERROR, STACK);
		(*ma)->errno = intern;
		return NULL;
	}	
	
	// Nastaveni hodnot
	stc->Top = NULL;
	stc->Length = 0;	
	return stc;
}

int stackPush(struct mainAll** ma, struct stack* stack, void* val){
	// kontrola na main_all strukturu
	if((*ma) == NULL){
		Log("Main_all = NULL", ERROR, STACK);
		(*ma)->errno = intern;
		return False;
	}
	
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		(*ma)->errno = intern;
		return False;
	}
	
	// vytvoreni nove polozky, naplneni daty a provazani s vrcholem
	struct stackItem* newitem = (struct stackItem*) gcMalloc(ma, sizeof(struct stackItem));
	if(newitem == NULL){
		Log("Allocation error", ERROR, STACK);
		(*ma)->errno = intern;
		return False;	
	}
	newitem->Value = val;
	newitem->Next = stack->Top;
	stack->Top = newitem;
	stack->Length++;
	return True;
}

void* stackPop(struct mainAll** ma, struct stack* stack){
	// kontrola na hlavni strukturu
	if((*ma) == NULL){
		Log("Main_all = NULL", ERROR, STACK);		
		(*ma)->errno = intern;
		return NULL;
	}
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		(*ma)->errno = intern;
		return NULL;
	}
	
	if(stack->Top != NULL){
		// provazani nasledujiciho na misto prvniho
		struct stackItem* item = stack->Top;
		stack->Top = stack->Top->Next;
		// kopie hodnoty navrcholu
		void* val = item->Value;
		// uvolneni prvku navrcholu
		gcFree(ma, item);
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

void* stackTop(struct mainAll** ma, struct stack* stack){
	// kontrola na hlavni strukturu
	if((*ma) == NULL){
		Log("Main_all = NULL", ERROR, STACK);		
		(*ma)->errno = intern;
		return NULL;
	}
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		(*ma)->errno = intern;
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

int stackEmpty(struct mainAll** ma, struct stack* stack){
	// kontrola na hlavni strukturu
	if((*ma) == NULL){
		Log("Main_all = NULL", ERROR, STACK);		
		(*ma)->errno = intern;
		return False;
	}
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("Stack == NULL", ERROR, STACK);
		(*ma)->errno = intern;
		return False;
	}

	return (stack->Length == 0);
}


