/**
 * SymbolTable.h
 *
 * Hlavickovy soubor pro knihovnu Tabulky symbolu.
 *
 */
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "Structs.h"


struct symbolTableNode* makeNewSymbolTable();
struct symbolTableNode* makeNewNamedNode(struct String*);
void* insertValue(struct symbolTableNode**, struct String*, struct String*);
int copyTable(struct symbolTableNode*, struct symbolTableNode**);
int deleteTable(struct symbolTableNode**);

void printSymbolTable(struct symbolTableNode*, int);
#endif
