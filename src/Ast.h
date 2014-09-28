#ifndef AST_H
#define AST_H

#include "Structs.h"

struct astNode* makeNewAST(struct mainAll**);
void printAst(struct astNode*);

#endif
