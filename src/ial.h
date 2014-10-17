#ifndef __IAL_H__
#define __IAL_H__

#include "Structs.h"

struct String* copy(struct String*, int, int);
int length(struct String*);
struct String* sort(struct String*);
int find(struct String*, struct String*);

// symbol table
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

#endif /* __IAL_H__ */

