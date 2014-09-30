/**
 * SymbolTable.h
 *
 * Hlavickovy soubor pro knihovnu Tabulky symbolu.
 *
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Structs.h"


struct symbolTableNode* makeNewSymbolTable(struct mainAll**);
struct symbolTableNode* makeNewNamedNode(struct mainAll**, struct String*);
int insertValue(struct mainAll**, struct symbolTableNode*, struct String*, struct String*);
int copyTable(struct mainAll**, struct symbolTableNode*, struct symbolTableNode**);
int deleteTable(struct mainAll**, struct symbolTableNode*);

int printSymbolTable(struct symbolTableNode*, int);
#endif
