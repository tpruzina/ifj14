#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Structs.h"
#include "String.h"
#include "ial.h"
#include "Log.h"
#include "GC.h"


#define E(x) { Log(x, ERROR, IAL); }

/**
 * INLINE funkce pro substring
 * --------------------
 * str: string pro ziskani dat
 * i:	pocatecni index
 * n:	delka vyrezavaneho retezce
 */
struct String* copy(struct String *str, int i, int n){
	if(str == NULL){
		E("Internal error - string from parameter is null");
		exit(intern);
	}
	if(str->Value == NULL){
		E("Internal error - string value is null");
		exit(intern);
	}

	// start index
	if(i <= 0){
		E("Semantic error - start index cannot be negative");
		exit(sem_else);
	}

	// length
	if(n <= 0){
		E("Semantic error - length of copy cannot be negative");
		exit(sem_else);
	}

	// nova instance
	struct String* new = makeNewString();
	
	// zarovnani na C-like indexovani
	i--;
		
	// kopirovani, dokud je co
	for(int j = 0; j < n; j++){
		if(j+i < str->Length){
			// pokud se novy index vleze jeste do rozmezi
			addChar(new, str->Value[j+i]);
		}		
		else
			break;
	}

	return new;
}

/**
 * INLINE funkce pro vraceni delky textu
 * --------------------
 * str: predany text
 */
int length(struct String* str){
	return str->Length;
}

void partition (struct String *str, int left, int right, int* i_, int* j_)
{
	int i = *i_;
	int j = *j_;

	char PM;
	int c; // pro presun

	i = left;
	j = right;

	PM = str->Value [(i+j)/2];

	do
	{
		while (str->Value [i] < PM) i++;
		while (str->Value [j] > PM) j--;

		if (i <= j)
		{
			c = str->Value [i];
			str->Value [i] = str->Value [j];
			str->Value [j] = c;

			i++;
			j--;
		}

	} while (i < j);

	*i_ = i;
	*j_ = j;

	return;
}

void quicksort (struct String *str, int left, int right)
{
	int i = 0;
	int j = 0;

	partition (str, left, right, &i, &j);
	if (left < j) quicksort (str, left, j);
	if (i < right) quicksort (str, i, right);

	return;
}

void sortString (struct String *str)
{
	// printf ("sortuju\n");

	// provedu pocatecni nastaveni
	int left = 0;
	int right = str->Length-1;

	// zavolam vlastni quicksort
	quicksort (str, left, right);

	return;
}

///**
// * Doprovodna funkce pro pocitani QuickSortu
// * --------------------
// * *A: pole hodnot
// * left: 	leva mez
// * right:	prava mez
// * *i: 		posuvne indexy
// * *j:		posuvne indexy
// */
//void partition(int *A, int left, int right, int *i, int *j){
//	int PM;
//	*i = left;
//	*j = right;
//
//	PM = A[(*i + *j)/2]; // pseudomedián
//
//	while(1){
//		while(A[*i] < PM){	// nic co je menší než PM mě nezajímá
//			*i+=1;
//		}
//		while(A[*j] > PM){	// tady mě nezajímá nic většího než PM
//			*j-=1;
//		}
//
//		// pokud se indexy nepřekřížili, prohodím prvky, na které ukazují
//		if(*i<*j){
//			// v přednáškách je i<=j, ale nemá cenu přehazovat prvek
//			// se sebou samým
//			int tmp = A[*i];
//			A[*i] = A[*j];
//			A[*j] = tmp;
//			*i+=1;
//			*j-=1;
//		}else{				// jinak skončím cyklus (repeat ... until(i>j)) v přednáškách until(i>=j),
//			break;			// ale nemá cenu přehazovat prvek se sebou samým
//		}
//	}
//}
//
///**
// * Funkce QuickSort
// * --------------------
// * *A: 		Pole na serazeni
// * left:	Leva mez
// * right:	Prava mez
// */
//void quickSort(int *A, int left, int right){
//	int i,j;
//	partition(A,left,right,&i,&j);	// rozdělím na dvě části
//
//	if(left<j){
//		quickSort(A,left,j);		// sortnu levou část
//	}
//
//	if(right>i){
//		quickSort(A,i,right);		// sortnu pravou část
//	}
//}
/**
 * INLINE funkce pro razeni retezcu
 * --------------------
 * str: string pro serazeni
 */
struct String* sort(struct String* src){
	if(src == NULL){
		E("Semantic error - string is null");
		exit(sem_else);
	}

	// udelat kopii
	struct String *res = makeNewString();
	copyString(src,&res);

	sortString(res);

	return res;
}

/**
 * INLINE funkce pro vyhledavani v retezcich
 * --------------------
 * text: 		podkladovy text pro vyhledavani
 * pattern: 	sablona textu, ktery se ma vyhledat
 */
int find(struct String* text, struct String* pattern){
	int result; // pozice, prvniho nalezu
      	
	int len_pattern = strlen(pattern->Value);
	int len_text = strlen(text->Value);
	
	// vektor fail
    int* fail = (int*)gcMalloc(sizeof(int) * len_pattern);
    // index hledaneho podretezce, index v retezci, dalsi pozice
    int pattern_inx, text_inx, next; 


	fail[0] = -1;


	//prochazim slovo - KMP graf
	for(int k = 1; k < len_pattern; k++){
		next = fail[k-1];

		while(next > -1 && (pattern->Value[next] != pattern->Value[k-1])){
			next = fail[next];
		}
		fail[k] = next+1;
    }

	//hledani podretezce
	text_inx = 0;
	pattern_inx = 0;

	while((text_inx < len_text) && (pattern_inx < len_pattern)){
		if(pattern_inx == -1 || (text->Value[text_inx] == pattern->Value[pattern_inx])){
			text_inx++;
			pattern_inx++;
		}
		else{
			//pujde podle pole fail
			pattern_inx=fail[pattern_inx];   
		}
	}

	if(pattern_inx == len_pattern){
		result = text_inx - len_pattern + 1;
	}
	else{
		result = text_inx;
	}

	return result;
}


//// SYMBOL TABLE

/**
 * Vytvori cisty uzel do tabulky. Bez jmena vyplneneho.
 */
struct symbolTableNode* makeNewSymbolTable(){	
	// alokace tabulky
	struct symbolTableNode* table = (struct symbolTableNode*)gcMalloc(sizeof(struct symbolTableNode));
	if(table == NULL){
		E("symTable: Allocation error");
		exit(intern);	
	}
	
	table->name = makeNewString();
	
	table->dataType = DT_NONE;

//	table->data.str_data = NULL;
//	table->data.int_data = 0;
//	table->data.real_data = 0;
//	table->data.bool_data = False;
//
//	table->left = NULL;
//	table->right = NULL;
	
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
	stn->init = NULL;
	
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
			E("insertValue: Redefinition!");
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
	
	(*table)->init = true;
	(*table)->dataType = DT_STR;
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
		Log("copyTable: src not null", DEBUG, IAL);
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
			case DT_NONE:
				Log("copyTable: unintialized variable", DEBUG, IAL);
				break;
			default:
				E("copy: Invalid data type");
				exit(sem_komp);
		}
		copyTable(src->left, &((*dest)->left));
		copyTable(src->right, &((*dest)->right));	
	}
	else {
		Log("copyTable: src == NULL!!!!", DEBUG, IAL);
		(*dest) = NULL;	
	}
	return True;
}
/**
 * Interni funkce jen pro tento soubor, maze jeden uzel tabulky.
 */
int delete(struct symbolTableNode* node){
	if(node == NULL){
		Log("delete: Cannot free NULL", WARNING, IAL);
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
		Log("deleteTable: Empty table", WARNING, IAL);
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
