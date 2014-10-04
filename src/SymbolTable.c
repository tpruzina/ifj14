#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Structs.h"
#include "GC.h"
#include "Stack.h"
#include "Log.h"
#include "SymbolTable.h"


/**
 * Vytvori cisty uzel do tabulky. Bez jmena vyplneneho.
 */
struct symbolTableNode* makeNewSymbolTable(){	
	// alokace tabulky
	struct symbolTableNode* table = (struct symbolTableNode*)gcMalloc(sizeof(struct symbolTableNode));
	if(table == NULL){
		Log("symTable: Allocation error", ERROR, SYMTABLE);
		global.errno = intern;
		return NULL;	
	}
	
	if((table->name = makeNewString()) == NULL){
		return NULL;
	}
	table->left = NULL;
	table->right = NULL;
	
	return table;
}
/**
 * Vytvori uzel pro zaznam do tabulky uz s definovanym jmenem.
 */
struct symbolTableNode* makeNewNamedNode(struct String* name){
	struct symbolTableNode* stn = makeNewSymbolTable();
	if(stn == NULL)
		return NULL;
		
	// kopirovani jmena do noveho
	if(!copyString(name, &(stn->name)))
		return NULL;
	
	
	stn->left = NULL;
	stn->right = NULL;
	
	return stn;
}

struct symbolTableNode* search(struct symbolTableNode** table, struct String* name){
	if(!(*table))
		return NULL;
		
	int key = strcmp((*table)->name->Value, name->Value);
	if(key == 0){
		return (*table);
	}
	else if(key < 0){
		// posun doleva	
		return search(&((*table)->left), name);
	}
	else if(key > 0){
		// posun doprava
		return search(&((*table)->right), name);
	}
	return NULL;
}

/**
 * Vklada polozku do tabulky symbolu.
 * Rekurzivnim zpusobem se zanori az na potrebne misto a vytvori novy uzel
 */
struct symbolTableNode* insertValue(struct symbolTableNode** table, struct String* name){
	if((*table) == NULL){
		// v pripade, ze se vklada do prazdne tabulky
		if(!((*table) = makeNewNamedNode(name))){
			return NULL;		
		}
		
		Log("insertValue: data inserted into empty table", DEBUG, SYMTABLE);
		
		printSymbolTable((*table), 0);
		return (*table);
	}
	else {
		int key = strcmp((*table)->name->Value, name->Value);
		
		if(key == 0){
			Log("insertValue: Redefinition!", ERROR, SYMTABLE);
			global.errno = sem_prog;
			return NULL;
		}
		else if(key < 0){
			// posun doleva || vytvoreni noveho uzlu
			if((*table)->left == NULL){
				if(!((*table)->left = makeNewNamedNode(name))){
					Log("insertValue: data inserted", DEBUG, SYMTABLE);
					printSymbolTable((*table), 0);
					return (*table)->left;				
				}
			}
			else 
				return insertValue(&((*table)->left), name);
			
		}
		else if(key > 0){
			// posun doprava || vytvoreni noveho uzlu
			if((*table)->right == NULL){
				if(!((*table)->right = makeNewNamedNode(name))){
					Log("insertValue: data inserted", DEBUG, SYMTABLE);
					printSymbolTable((*table), 0);
					return (*table)->right;				
				}
			}
			else
				return insertValue(&((*table)->right), name);
		}
	}

	return NULL;
}

/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do integeru
 */
int insertDataInteger(struct symbolTableNode** table, int value){
	if((*table) == NULL)
		return False;
		
	(*table)->type = DT_INT;
	(*table)->data.int_data = value;
	return True;
}
/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do realu
 */
int insertDataReal(struct symbolTableNode** table, double value){
	if((*table) == NULL)
		return False;
		
	(*table)->type = DT_REAL;
	(*table)->data.real_data = value;
	return True;
}
/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do boolu
 */
int insertDataBoolean(struct symbolTableNode** table, bool value){
	if((*table) == NULL)
		return False;
		
	(*table)->type = DT_BOOL;
	(*table)->data.bool_data = value;
	return True;
}
/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do stringu
 */
int insertDataString(struct symbolTableNode** table, struct String* value){
	if((*table) == NULL)
		return False;
		
	if(value == NULL)
		return False;
	
	(*table)->type = DT_STR;
	if(!(copyString(value, &((*table)->data.str_data))))
		return False;
	return True;
}

/**
 * Kopiruje celou tabulku z src do dest parametru. Rekurzivnim zpusobem.
 * Predpokladane pouziti v TOP vrstve zasobniku tabulek symbolu.
 */
int copyTable(struct symbolTableNode* src, struct symbolTableNode** dest){
	if(src != NULL){
		(*dest) = makeNewNamedNode(src->name);
		
		// nemusi byt kopirovano nic		
		if(src->data.str_data != NULL)
			copyString(src->data.str_data, &((*dest)->data.str_data));	
		else
			(*dest)->data.str_data = NULL;
	
		(*dest)->data.int_data = src->data.int_data;
		(*dest)->data.real_data = src->data.real_data;
		(*dest)->data.bool_data = src->data.bool_data;

		copyTable(src->left, &((*dest)->left));
		copyTable(src->right, &((*dest)->right));	
	}
	else
		(*dest) = NULL;	
		
	return True;
}
/**
 * Interni funkce jen pro tento soubor, maze jeden uzel tabulky.
 */
int delete(struct symbolTableNode* node){
	if(node == NULL){
		Log("Cannot free NULL", WARNING, SYMTABLE);
		return False;
	}
	
	if(node->name != NULL)
		gcFree(node->name);
	gcFree(node);
	
	return True;
}

/**
 * Maze celou tabulku, rekurzivnim zpusobem, postfixovym postupem.
 */
int deleteTable(struct symbolTableNode** table){
	// kontrola na neprazdnost 
	if(*table == NULL){
		Log("Empty table", WARNING, SYMTABLE);
		return False;
	}	
	
	if((*table)->left != NULL)
		deleteTable(&((*table)->left));
	if((*table)->right != NULL)
		deleteTable(&((*table)->right));
		
	delete((*table));
	(*table) = NULL;
	return True;	
}

/**
 * Vytiskne graficky vzhled tabulky symbolu.
 */
void printSymbolTable(struct symbolTableNode* table, int lvl){
	if(table != NULL){
		if(table->left != NULL)
			printSymbolTable(table->left, lvl+1);
		
		for (int i = 0; i < lvl; i++)
			fprintf(stderr, "\t");
		fprintf(stderr, "[ %s ]\n", table->name->Value);
		
		if(table->right != NULL)
			printSymbolTable(table->right, lvl+1);
	}
}
