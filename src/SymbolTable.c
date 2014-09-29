#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "GC.h"
#include "Stack.h"
#include "Log.h"

struct symbolTableNode* makeNewSymbolTable(struct mainAll** ma){
	if((*ma) == NULL){
		Log("Main_all == NULL", ERROR, SYMTABLE);
		return NULL;	
	}
	
	// alokace tabulky
	struct symbolTableNode* table = (struct symbolTableNode*)gcMalloc(ma, sizeof(struct symbolTableNode));
	if(table == NULL){
		Log("Allocation error", ERROR, SYMTABLE);
		(*ma)->errno = intern;
		return NULL;	
	}
	
	if((table->name = makeNewString(ma)) == NULL){
		return False;
	}
	table->left = NULL;
	table->right = NULL;
	
	return table;
}

struct symbolTableNode* makeNewNamedNode(struct mainAll** ma, struct String* name, struct String* value){
	struct symbolTableNode* stn = makeNewSymbolTable(ma);
	if(stn == NULL)
		return NULL;
		
	// kopirovani jmena do noveho
	if(!copyString(ma, name, &(stn->name)))
		return NULL;
	
	// kopirovani hodnoty, neni nutne...
	if(value != NULL){
		if(!copyString(ma, value, &(stn->value)))
			return NULL;
	}
	stn->left = NULL;
	stn->right = NULL;
	
	return stn;
}

int insertValue(struct mainAll** ma, struct symbolTableNode* table, struct String* name, struct String* value){
	if(table == NULL){
		// chyba algoritmu
		Log("Alg error - insertValue", ERROR, SYMTABLE);
		(*ma)->errno = intern;
		return False;		
	}
	else {
		int key = compareStrings(table->name, name);
		
		if(key == 0){
			Log("Redefinition!", ERROR, SYMTABLE);
			(*ma)->errno = sem_prog;
			return False;
		}
		else if(key < 0){
			// posun doleva || vytvoreni noveho uzlu
			if(table->left == NULL){
				if(!(table->left = makeNewNamedNode(ma, name, value)))
					return False;				
			}
			else
				insertValue(ma, table->left, name, value);
		}
		else if(key > 0){
			// posun doprava || vytvoreni noveho uzlu
			if(table->right == NULL){
				if(!(table->right = makeNewNamedNode(ma, name, value)))
					return False;				
			}
			else
				insertValue(ma, table->right, name, value);
		}
	}

	return True;
}
