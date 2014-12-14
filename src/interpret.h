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

#ifndef _INTERPRET_H
#define _INTERPRET_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "gc.h"
#include "structs.h"
#include "log.h"
#include "stack.h"
#include "ast.h"
#include "ial.h"
#include "scanner.h"
#include "parser.h"

void interpret();
void *runTree(struct astNode *curr);

/* ARITMETICKE OPERACE +,-,... */
struct symbolTableNode *arithmetic(struct symbolTableNode *left,struct symbolTableNode *right,int op);
bool compare(struct symbolTableNode *left,struct symbolTableNode *right,int op);

/* BUILTIN FUNKCIE */
struct symbolTableNode *btnCopy(struct queue *pars);
struct symbolTableNode *btnSort(struct queue *pars);
struct symbolTableNode *btnFind(struct queue *pars);
struct symbolTableNode *btnLength(struct queue *pars);
void writeNode(struct astNode *p);
void readNode(struct symbolTableNode *p);

/* POMOCNE FUNKCIE */
struct symbolTableNode* searchST(struct symbolTableNode** table, struct String* name);
struct symbolTableNode convertAST2STN(struct astNode *ast);
// pomocna funkcie pre volania fcii - zobere varspars a nakopiruje ich do ST
struct symbolTableNode *pushVarsParsIntoTable(
		struct queue *call_params,
		struct queue *function_params,
		struct queue *function_vars
);

#endif
