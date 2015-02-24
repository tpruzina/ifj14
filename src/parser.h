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

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

int parser();

struct astNode* parseProgram();
struct astNode* parseBody(struct toc**, bool, int);
struct queue* parseVars(struct toc**);
struct queue* parseParams();
struct astNode* parseFunction();
struct queue* parseCallParams(struct toc**);
struct astNode* parseFuncCall(struct toc**);

struct astNode* ifStatement(struct toc**);
struct astNode* whileStatement(struct toc**);
struct astNode* repeatStatement(struct toc**);

struct astNode* parseCommand(struct toc** cur);
struct astNode* parseExpression(struct toc**);

void controlCallParams(struct queue*, struct queue*);
void controlDefinitionParams(struct queue*, struct queue*);
int checkFunctionDeclarations(struct symbolTableNode*);
struct astNode* arrayIndex(struct toc**, struct String*);
#endif /* __PARSER_H__ */

