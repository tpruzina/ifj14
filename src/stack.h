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

#ifndef STACK_H
#define STACK_H

#include "string.h"

/**
 * Struktura prvku zasobniku
 */
struct stackItem {
	// int priority
	void* Value;
	struct stackItem* Next;
};

/**
 * Struktura zasobniku
 */
struct stack {
	struct stackItem* Top;
	int Length;
};


struct stack* makeNewStack();
int stackPush(struct stack*, void*);
void* stackPop(struct stack*);
void* stackTop(struct stack*);
int stackEmpty(struct stack*);


struct queueItem {
	void* value;
	struct queueItem* next;
};
struct queue {
	int length;
	struct queueItem* start;
	struct queueItem* end;
};
struct queue* makeNewQueue();
int queuePush(struct queue*, void*);
void* queuePop(struct queue*);
void* queueTop(struct queue*);
int queueEmpty(struct queue*);

#endif
