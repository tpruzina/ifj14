/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativního jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antonín    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juřík Martin     <xjurik08@stud.fit.vutbr.cz           ****
****               David Petr       <xdavid15@stud.fit.vutbr.cz>          ****
****                                                                      ****
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"
#include "log.h"
#include "gc.h"
#include "structs.h"

/**
 *	Konstanta pro snadny prevod UPPERCASE do lowercase
 */
const int TOLOWER = (int)('a' - 'A');

/**
 * Vytvari novou strukturu a vraci ji.
 * ------------------------------------------------------------------
 */
struct String* makeNewString(){
	struct String* str = (struct String*)gcMalloc(sizeof(struct String));
	if(str == NULL){
		Log("String: makeNewString: malloc error", ERROR, STRING);
		exit(intern);
	}

	str->Value = (char*)gcMalloc(sizeof(char));
	str->Value[0] = '\0';
	
	str->Length = 0;
	str->Allocated = 1;

	return str;
}

/**
 * Pridava na konec stringu znak, zvetsi pole a ulozi.
 * ------------------------------------------------------------------
 * @param s: odkaz na string, do ktereho chceme pridavat znak
 * @param c: znak pro pridani
 */
int addChar(struct String* s, char c){
	if(s == NULL){
		exit(intern);
	}
	
	if(s->Value == NULL){
		// v pripade, ze string sice alokovan byl, ale neobsahoval hodnotu
		// -> alokovat pole a vlozit znak + \0 nakonec
		s->Value = (char*)gcMalloc(sizeof(char)*2);

		s->Value[0] = c;
		s->Value[1] = '\0';
		
		s->Allocated = 2;
		s->Length = 1;

		//Log("String: addChar: pridani znaku (alokace)", DEBUG, STRING);
		return True;
	}
	else {
		// v pripade, ze se pouze pridava znak do pole
		s->Value = (char*)gcRealloc(s->Value, s->Allocated + 1);
		s->Value[s->Length] = c;
		s->Value[s->Length+1] = '\0';
		
		s->Allocated += 1;
		s->Length += 1;

		//Log("String: addChar: pridani znaku (realokace)", DEBUG, STRING);
		return True;
	}

	Log("String: addChar: neco probehlo spatne", WARNING, STRING);
	return False;
}

/**
 * Vycisti string, vynuluje obsah.
 * ------------------------------------------------------------------
 * @param str: odkaz na string, ktery chceme vycistit
 */
int emptyString(struct String* str){
	if(str == NULL){
		Log("String is NULL, cannot be cleaned", ERROR, STRING);
		exit(intern);
	}
	if(str->Value != NULL){
		str->Value = (char*)gcRealloc(str->Value, 1);
		str->Value[0] = '\0';
		str->Allocated = 1;
		str->Length = 0;
	}
	return True;
}

/**
 * Uvolni vytvoreny string, vcetne odepsani z GC.
 * Kontroly na spravne predany string.
 * ------------------------------------------------------------------
 * @param s: odkaz na strukturu, kterou chceme smazat
 */
int freeString(struct String* s){
	if(s == NULL){
		Log("String: freeString: predany string je prazdny", WARNING, STRING);
		return False;
	}

	gcFree(s);
	return True;
}
/**
 * Porovna string s charovym polem
 * ------------------------------------------------------------------
 * @param s: prvni structura stringu
 * @param s2: druhe pole znaku
 */
int compareString(struct String* s, char* s2){
	return strcmp(s->Value,s2);
}
/**
 * Porovnava obsahy dvou Stringovych struktur.
 * ------------------------------------------------------------------
 * @param s1: odkaz na strukturu stringu kterou chceme porovnat
 * @param s2: odkaz na strukturu stringu druhou
 */
int compareStrings(struct String* s1, struct String* s2){
	return strcmp(s1->Value, s2->Value);
}

/**
 * Skopiruje data ze stringu do druheho
 * ------------------------------------------------------------------
 * @param src: odkaz na strukturu stringu odkud bude kopirovat
 * @param dest: odkaz na strukturu, kam bude obsah kopirovat
 */
int copyString(struct String* src, struct String** dest){
	if(src == NULL){
		Log("Src string == empty", ERROR, STRING);
		exit(intern);	
	}
	
	if((*dest) == NULL && ((*dest) = makeNewString()) == NULL){
		Log("Destination string cannot be maked", ERROR, STRING);
		exit(intern);
	}
	else {
		if(!emptyString(*dest)){
			return False;
		}
	}

	for(int i = 0; i < src->Length; i++){
		if(!addChar((*dest), src->Value[i])){
			return False;
		}
	}
	
	return True;
}

