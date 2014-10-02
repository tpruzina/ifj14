#ifndef STACK_H
#define STACK_H

#include "String.h"

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
	struct queueItem* start;
	struct queueItem* end;
};
struct queue* makeNewQueue();
int queuePush(struct queue*, void*);
void* queuePop(struct queue*);
void* queueTop(struct queue*);
int queueEmpty(struct queue*);

#endif
