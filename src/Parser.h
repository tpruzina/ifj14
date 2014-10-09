#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>

#include "Structs.h"

int parser();

struct astNode* parseProgram();
struct astNode* parseBody(struct toc**);
struct queue* parseVars(struct toc**);
struct queue* parseParams();
struct astNode* parseFunction();
struct queue* parseCallParams(struct toc**);
struct astNode* parseFuncCall(struct toc**);

struct astNode* ifStatement(struct toc**);
struct astNode* whileStatement(struct toc**);
struct astNode* repeatStatement(struct toc**);

struct astNode* parseCommand();
struct astNode* parseExpression(struct toc**);

void controlCallParams(struct queue*, struct queue*);
void controlDefinitionParams(struct queue*, struct queue*);
#endif /* __PARSER_H__ */

