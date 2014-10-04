#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>

#include "Structs.h"

struct astNode* parseProgram();
struct astNode* parseParams();
struct astNode* parseVars(struct toc**);
struct astNode* parseCallParams();
struct astNode* parseFuncCall(struct toc*);
struct astNode* parseFunction();
struct astNode* parseBody(int);
struct astNode* parseCommand();
struct astNode* parseExpression(int);

#endif /* __PARSER_H__ */

