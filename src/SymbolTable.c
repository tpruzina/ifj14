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
		exit(intern);	
	}
	
	table->name = makeNewString();
	if(table->name == NULL)
		return NULL;
	
	table->data.str_data = NULL;
	table->data.int_data = 0;
	table->data.real_data = 0;
	table->data.bool_data = False;
	
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
struct symbolTableNode* insertValue(struct symbolTableNode** table, struct String* name, int dtype){
	if((*table) == NULL){
		// v pripade, ze se vklada do prazdne tabulky
		if(!((*table) = makeNewNamedNode(name))){
			return NULL;		
		}		
		(*table)->dataType = dtype;
		(*table)->init = false;
		//printSymbolTable((*table), 0);
		return (*table);
	}
	else {
		int key = strcmp((*table)->name->Value, name->Value);
		
		if(key < 0){
			// posun doleva || vytvoreni noveho uzlu
			if((*table)->left == NULL){
				if(!((*table)->left = makeNewNamedNode(name)))
					return NULL;
				
				(*table)->left->dataType = dtype;
				//printSymbolTable((*table), 0);
				return (*table)->left;				
			}
			else 
				return insertValue(&((*table)->left), name, dtype);
		}
		else if(key > 0){
			// posun doprava || vytvoreni noveho uzlu
			if((*table)->right == NULL){
				if(!((*table)->right = makeNewNamedNode(name)))
					return NULL;	
				
				(*table)->right->dataType = dtype;
				//printSymbolTable((*table), 0);
				return (*table)->right;				
			}
			else
				return insertValue(&((*table)->right), name, dtype);
		}
		else {
			Log("insertValue: Redefinition!", ERROR, SYMTABLE);
			exit(sem_prog);
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
		
	(*table)->dataType = DT_INT;
	(*table)->data.int_data = value;
	(*table)->init = true;
	return True;
}
/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do realu
 */
int insertDataReal(struct symbolTableNode** table, double value){
	if((*table) == NULL)
		return False;
		
	(*table)->dataType = DT_REAL;
	(*table)->data.real_data = value;
	(*table)->init = true;
	return True;
}
/**
 * Vklada do vytvoreneho uzlu data na prislusnou polozku do unionu.
 * Kopiruje data do boolu
 */
int insertDataBoolean(struct symbolTableNode** table, bool value){
	if((*table) == NULL)
		return False;
		
	(*table)->dataType = DT_BOOL;
	(*table)->data.bool_data = value;
	(*table)->init = true;
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
	
	(*table)->dataType = DT_STR;
	if(!(copyString(value, &((*table)->data.str_data))))
		return False;
		
	(*table)->init = true;
	return True;
}

/**
 * Kopiruje celou tabulku z src do dest parametru. Rekurzivnim zpusobem.
 * Predpokladane pouziti v TOP vrstve zasobniku tabulek symbolu.
 */
int copyTable(struct symbolTableNode* src, struct symbolTableNode** dest){
	if(src != NULL){
		(*dest) = makeNewNamedNode(src->name);
		
		
		(*dest)->dataType = src->dataType;
		switch(src->dataType){
			case DT_STR:	
				// nemusi byt kopirovano nic		
				if(src->data.str_data != NULL)
					copyString(src->data.str_data, &((*dest)->data.str_data));	
				else
					(*dest)->data.str_data = NULL;
				break;
			case DT_INT:	
				(*dest)->data.int_data = src->data.int_data;
				break;
			case DT_REAL:
				(*dest)->data.real_data = src->data.real_data;
				break;
			case DT_BOOL:
				(*dest)->data.bool_data = src->data.bool_data;
				break;
			default:
				Log("copy: Invalid data type", ERROR, PARSER);
				exit(sem_komp);
		}
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
		Log("delete: Cannot free NULL", WARNING, SYMTABLE);
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
		Log("deleteTable: Empty table", WARNING, SYMTABLE);
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
	if(!PRT) return;

	if(table != NULL){
		if(table->left != NULL)
			printSymbolTable(table->left, lvl+1);
		
		for (int i = 0; i < lvl; i++)
			fprintf(stderr, "  ");
		switch(table->dataType){
			case DT_INT:
				fprintf(stderr, "%s[ %s | %d | INT ]%s\n", COLOR_GRN, table->name->Value, table->data.int_data, COLOR_NRM);
				break;
			case DT_REAL:
				fprintf(stderr, "%s[ %s | %g | REAL ]%s\n", COLOR_BLU, table->name->Value, table->data.real_data, COLOR_NRM);
				break;
			case DT_BOOL:
				fprintf(stderr, "%s[ %s | %s | BOOL ]%s\n", COLOR_MAG, table->name->Value, (table->data.bool_data)?"True":"False", COLOR_NRM);
				break;
			case DT_STR:
				fprintf(stderr, "%s[ %s | \"%s\" | STR ]%s\n", COLOR_YEL, table->name->Value, (table->data.str_data)?table->data.str_data->Value:"NULL", COLOR_NRM);
				break;
		}	
			
		if(table->right != NULL)
			printSymbolTable(table->right, lvl+1);
		
		return;
	}
	fprintf(stderr,"%s empty %s\n", COLOR_LRED, COLOR_NRM);
}
