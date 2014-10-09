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
struct symbolTableNode* search(struct symbolTableNode**, struct String*);
struct symbolTableNode* insertValue(struct symbolTableNode**, struct String*, int);
int insertDataInteger(struct symbolTableNode** table, int value);
int insertDataReal(struct symbolTableNode** table, double value);	
int insertDataBoolean(struct symbolTableNode** table, bool value);	
int insertDataString(struct symbolTableNode** table, struct String* value);
int copyTable(struct symbolTableNode*, struct symbolTableNode**);
int deleteTable(struct symbolTableNode**);

void printSymbolTable(struct symbolTableNode*, int);


struct String *generateUniqueID(void);

#endif
