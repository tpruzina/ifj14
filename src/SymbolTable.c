#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "GC.h"
#include "Stack.h"
#include "Log.h"

/**
 * Vytvori cisty uzel do tabulky. Bez jmena vyplneneho.
 */
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
/**
 * Vytvori uzel pro zaznam do tabulky uz s definovanym jmenem.
 */
struct symbolTableNode* makeNewNamedNode(struct mainAll** ma, struct String* name){
	struct symbolTableNode* stn = makeNewSymbolTable(ma);
	if(stn == NULL)
		return NULL;
		
	// kopirovani jmena do noveho
	if(!copyString(ma, name, &(stn->name)))
		return NULL;
	
	
	stn->left = NULL;
	stn->right = NULL;
	
	return stn;
}
/**
 * Vklada polozku do tabulky symbolu.
 * Rekurzivnim zpusobem se zanori az na potrebne misto a vytvori novy uzel
 */
int insertValue(struct mainAll** ma, struct symbolTableNode* table, struct String* name, struct String* value){
	if(table == NULL){
		// chyba algoritmu -- nemuze vkrocit do NULLu
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
				if(!(table->left = makeNewNamedNode(ma, name)))
					return False;				
			}
			else
				return insertValue(ma, table->left, name, value);
		}
		else if(key > 0){
			// posun doprava || vytvoreni noveho uzlu
			if(table->right == NULL){
				if(!(table->right = makeNewNamedNode(ma, name)))
					return False;				
			}
			else
				return insertValue(ma, table->right, name, value);
		}
	}

	return True;
}
/**
 * Kopiruje celou tabulku z src do dest parametru. Rekurzivnim zpusobem.
 * Predpokladane pouziti v TOP vrstve zasobniku tabulek symbolu.
 */
int copyTable(struct mainAll** ma, struct symbolTableNode* src, struct symbolTableNode** dest){
	if(src != NULL){
		(*dest) = makeNewNamedNode(ma, src->name);
		
		// nemusi byt kopirovano nic		
		if(src->data.str_data != NULL)
			copyString(ma, src->data.str_data, &((*dest)->data.str_data));	
		else
			(*dest)->data.str_data = NULL;
	
		(*dest)->data.int_data = src->data.int_data;
		(*dest)->data.real_data = src->data.real_data;
		(*dest)->data.bool_data = src->data.bool_data;

		copyTable(ma, src->left, &((*dest)->left));
		copyTable(ma, src->right, &((*dest)->right));	
	}
	else
		(*dest) = NULL;	
		
	return True;
}
/**
 * Interni funkce jen pro tento soubor, maze jeden uzel tabulky.
 */
int delete(struct mainAll** ma, struct symbolTableNode* node){
	if(node == NULL){
		Log("Cannot free NULL", WARNING, SYMTABLE);
		return False;
	}
	
	if(node->name != NULL)
		gcFree(ma, node->name);
	gcFree(ma, node);
	
	return True;
}

/**
 * Maze celou tabulku, rekurzivnim zpusobem, postfixovym postupem.
 */
int deleteTable(struct mainAll** ma, struct symbolTableNode* table){
	// kontrola na neprazdnost 
	if(table == NULL){
		Log("Empty table", WARNING, SYMTABLE);
		return False;
	}	
	
	if(table->left != NULL)
		deleteTable(ma, table->left);
	if(table->right != NULL)
		deleteTable(ma, table->right);
		
	
	delete(ma, table);
	return True;	
}
/*
int removeValue(struct mainAll** ma, struct symbolTableNode* table, struct String* name){
	if(table == NULL){
		Log("Alg error - removeValue", ERROR, SYMTABLE);
		(*ma)->errno = intern;
		return False;	
	}
	else {
		// nerekurzivni nalezeni prvku
		
		struct symbolTableNode* cur = table;
		struct symbolTableNode* par = NULL;
		int key = compareStrings(cur->name, name);
		while(key != 0){
			// zaloha 
			par = cur;
			
			// posun aktualniho prvku na dalsi
			if(key < 0 && cur->left != NULL)
				cur = cur->left;
			else if (key > 0 && cur->right != NULL)
				cur = cur->right;			
			else if(cur->left == NULL && cur->right == NULL)
				break;
			else 
				break;
				
			// ulozeni noveho postupu kam
			key = compareStrings(cur->name, name);
		}	
		
		if(cur != NULL && (par->left == cur || par->right == cur)){
			// nalezeni prvku
			
			if(cur->left == NULL && cur->right == NULL){
				// nema poduzly -> smazat hned
				
				if(par->left == cur)
					par->left = NULL;
				else if(par->right == cur)
					par->right = NULL;
					
				delete(ma, cur);					
			}	
			else if(cur->left != NULL && cur->right == NULL){
				// ma levy poduzel -> nahradit vrcholem
								
				if(par->left == cur)
					par->left = cur->left;
				else if(par->right == cur)
					par->right = cur->left;
					
				delete(ma, cur);
			}
			else if(cur->left == NULL && cur->right != NULL){
				// ma pravy poduzel -> nahradit vrcholem
			
				if(par->left == cur)
					par->left = cur->right;
				else if(par->right == cur)
					par->right = cur->right;
					
				delete(ma, cur);
			}
			else {
				// ma oba poduzly -> nejpravejsi leveho podstromu
				// TODO dodelat!
				struct symbolTableNode* node = cur->left;
				struct symbolTableNode* prenode;
				while(node->right){
					prenode = node;
					node = node->right;				
				}
				
				// zruseni odkazu doprava na nejpravejsi prvek
				if(prenode != NULL && prenode->right == node)
					prenode->right = NULL;
					
				
				node->right = cur->right;
				
					
			}
		}	
	}
	return True;

}*/

int printSymbolTable(struct symbolTableNode* table, int lvl){
	if(table != NULL){
		if(table->left != NULL)
			printSymbolTable(table->left, lvl+1);
		
		for (int i = 0; i < lvl; i++)
			fprintf(stderr, " ");
		fprintf(stderr, "[ %s ]\n", table->name->Value);
		
		if(table->right != NULL)
			printSymbolTable(table->right, lvl+1);
			
		return True;
	}
	
	return False;
}
