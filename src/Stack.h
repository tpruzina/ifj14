#ifndef STACK_H
#define STACK_H

#include "String.h"

struct stackItem {
	// int priority
	void* Value;
	struct stackItem* Next;
};

struct stack {
	struct stackItem* Top;
	int Length;
};


struct stack* makeNewStack(struct mainAll**);
int stackPush(struct mainAll**, struct stack*, void*);
void* stackPop(struct mainAll**, struct stack*);
void* stackTop(struct mainAll**, struct stack*);
int stackEmpty(struct mainAll**, struct stack*);
#endif
