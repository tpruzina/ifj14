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


struct stack* makeNewStack();
int stackPush(struct stack*, void*);
void* stackPop(struct stack*);
void* stackTop(struct stack*);
int stackEmpty(struct stack*);
#endif
