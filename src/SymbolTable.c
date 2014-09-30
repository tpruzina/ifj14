#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "GC.h"
#include "Stack.h"
#include "Log.h"

/**
 * Vytvori cisty uzel do tabulky. Bez jmena vyplneneho.
 */
struct symbolTableNode* makeNewSymbolTable(){	
	// alokace tabulky
	struct symbolTableNode* table = (struct symbolTableNode*)gcMalloc(sizeof(struct symbolTableNode));
	if(table == NULL){
		Log("Allocation error", ERROR, SYMTABLE);
		global.errno = intern;
		return NULL;	
	}
	
	if((table->name = makeNewString()) == NULL){
		return False;
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
/**
 * Vklada polozku do tabulky symbolu.
 * Rekurzivnim zpusobem se zanori az na potrebne misto a vytvori novy uzel
 */
int insertValue(struct symbolTableNode** table, struct String* name, struct String* value){
	if((*table) == NULL){
		if(!((*table) = makeNewNamedNode(name))){
			return False;		
		}
	}
	else {
		int key = compareStrings((*table)->name, name);
		
		if(key == 0){
			Log("Redefinition!", ERROR, SYMTABLE);
			global.errno = sem_prog;
			return False;
		}
		else if(key < 0){
			// posun doleva || vytvoreni noveho uzlu
			if((*table)->left == NULL){
				if(!((*table)->left = makeNewNamedNode(name)))
					return False;				
			}
			else
				return insertValue(&((*table)->left), name, value);
		}
		else if(key > 0){
			// posun doprava || vytvoreni noveho uzlu
			if((*table)->right == NULL){
				if(!((*table)->right = makeNewNamedNode(name)))
					return False;				
			}
			else
				return insertValue(&((*table)->right), name, value);
		}
	}

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
