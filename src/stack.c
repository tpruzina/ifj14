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
#include "structs.h"
#include "log.h"
#include "stack.h"
#include "gc.h"

/**
 * Vytvori novou strukturu zasobniku. S vynulovanym vrcholem a nulovou delkou.
 * ------------------------------------------------------------------
 */
struct stack* makeNewStack(){
	// alokace nove polozky
	struct stack* stc = (struct stack*)gcMalloc(sizeof(struct stack));
	if(stc == NULL){
		Log("Allocation error", ERROR, STACK);
		exit(intern);
	}	
	
	// Nastaveni hodnot
	stc->Top = NULL;
	stc->Length = 0;	
	return stc;
}

/**
 * Vlozi na vrchol zasobniku novy uzel.
 * ------------------------------------------------------------------
 * @param stack: odkaz na strukturu zasobniku, do ktereho chceme ukladat
 * @param val: odkaz na to co chceme pridat
 */
int stackPush(struct stack* stack, void* val){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackPush: Stack == NULL", ERROR, STACK);
		exit(intern);
	}
	
	// vytvoreni nove polozky, naplneni daty a provazani s vrcholem
	struct stackItem* newitem = (struct stackItem*) gcMalloc(sizeof(struct stackItem));
	if(newitem == NULL){
		Log("stackPush: Allocation error", ERROR, STACK);
		exit(intern);	
	}
	newitem->Value = val;
	newitem->Next = stack->Top;
	stack->Top = newitem;
	stack->Length++;
	return True;
}

/**
 * Odstrani a vrati vrchol zasobniku.
 * ------------------------------------------------------------------
 * @param stack: odkaz na strukturu zasobniku
 */
void* stackPop(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackPop: Stack == NULL", ERROR, STACK);
		exit(intern);
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
 * ------------------------------------------------------------------
 * @param stack: odkaz na strukturu zasobniku
 */
void* stackTop(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackTop: Stack == NULL", ERROR, STACK);
		exit(intern);
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
 * ------------------------------------------------------------------
 * @param stack: odkaz na strukturu zasobniku
 */
int stackEmpty(struct stack* stack){
	// kontroly na parametr se zasobnikem
	if(stack == NULL){
		Log("stackEmpty: Stack == NULL", ERROR, STACK);
		exit(intern);
	}

	return (stack->Length == 0);
}

/**
 * Vytvari novou instanci fronty
 * ------------------------------------------------------------------
 */
struct queue* makeNewQueue(){
	struct queue* que = (struct queue*)gcMalloc(sizeof(struct queue));
	if(que == NULL){
		Log("queue: Allocation error", ERROR, STACK);
		exit(intern);
	}

	que->start = NULL;
	que->end = NULL;
	que->length = 0;
	return que;
}
/**
 * Vklada do fronty nakonec prvek
 * ------------------------------------------------------------------
 * @param que: fronta, do ktere ma byt prvek vlozen
 * @param value: hodnota, ktera ma byt vlozena
 */
int queuePush(struct queue* que, void* value){
	if(que == NULL)
		return False;
		
	struct queueItem* item = (struct queueItem*)gcMalloc(sizeof(struct queueItem));
	if(item == NULL){
		Log("queue: Allocation error", ERROR, STACK);
		exit(intern);
	} 	
	item->value = value;
	item->next = NULL;
	
	if(que->start == NULL)
		que->start = item;
	else 
		que->end->next = item;
	que->end = item;
	que->length += 1;
	return True;
}
/**
 * Odstrani z vrcholu prvek a vrati jeho hodnotu
 * ------------------------------------------------------------------
 * @param que: fronta na odstraneni prvku
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
	
	que->length -= 1;
	return ptr;
}
/**
 * Vraci hodnotu na vrcholu fronty, bez toho aby ho odstranila
 * ------------------------------------------------------------------
 * @param que: fronta
 */
void* queueTop(struct queue* que){
	if(que == NULL)	
		return NULL;
	
	return que->start->value;	
}
/**
 * Vraci True v pripade, ze se prvni i posledni prvek rovnaji a jsou NULL
 * ------------------------------------------------------------------
 * @param que: fronta pro zjisteni
 */
int queueEmpty(struct queue* que){
	if(que == NULL)
		return -1;
	
	if(que->start == que->end && que->start == NULL && que->length == 0)
		return True;
	else 
		return False;
}
