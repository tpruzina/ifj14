#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>

#include "Structs.h"

int parser();

struct astNode* parseProgram();
struct astNode* parseBody(struct toc**);
struct astNode* parseVars(struct toc**);
struct astNode* parseParams();
struct astNode* parseFunction();
struct astNode* parseCallParams();
struct astNode* parseFuncCall(struct toc*);

struct astNode* ifStatement(struct toc**);
struct astNode* whileStatement();
struct astNode* repeatStatement(struct toc**);

struct astNode* parseCommand();
struct astNode* parseExpression(struct toc**);

#endif /* __PARSER_H__ */

