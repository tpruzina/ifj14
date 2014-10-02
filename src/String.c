/*
 * String.c
 *
 * Modul pro praci s textem, umi pridavat a odebirat znaky, porovnavat atd.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "Log.h"
#include "GC.h"
#include "Structs.h"

const int TOLOWER = (int)('a' - 'A');

/**
 * Vytvari novou strukturu a vraci ji.
 */
struct String* makeNewString(){
	struct String* str = (struct String*)gcMalloc(sizeof(struct String));
	if(str == NULL){
		Log("String: makeNewString: malloc error", ERROR, STRING);
		return NULL;
	}

	str->Value = NULL;
	str->Length = 0;
	str->Allocated = 0;

	return str;
}

/**
 * Pridava na konec stringu znak, zvetsi pole a ulozi.
 */
int addChar(struct String* s, char c){
	if(s == NULL){
		// v pripade, ze dostane prazdny string -> zalozi novy a prida znak
		s = makeNewString();
		Log("String: addChar: novy string", DEBUG, STRING);
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
 */
int emptyString(struct String* str){
	if(str == NULL){
		Log("String is NULL, cannot be cleaned", ERROR, STRING);
		return False;
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
 * Tiskne obsah struktury Stringu
 */
int printString(struct String* s){
	if(s == NULL){
		return False;
	}
	printf("String:\n");
	printf("   Value:    \t\"%s\"\n", s->Value);
	printf("   Length:   \t%d\n", s->Length);
	printf("   Allocated:\t%d\n", s->Allocated);
	
	return False;
}

/**
 * Uvolni vytvoreny string, vcetne odepsani z GC.
 * Kontroly na spravne predany string.
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
 * Zastupuje strcmp
 */
int compareCharArrays(char* s1, char* s2){
	if(s1 == NULL || s2 == NULL){
		Log("Char arrays are both NULL", ERROR, STRING);
		return False;
	}
	
	Log("All done", DEBUG, STRING);
	int len = getCharArrayLength(s1);
	int len2 = getCharArrayLength(s2);
	if(len == len2){
		for(int i = 0; i < len; i++){
			if(s1[i] != s2[i]){
				if(s1[i] < s2[i])
					return -1;
				else
					return 1;			
			}
		}
		
		Log("Char arrays are same", DEBUG, STRING);
		return 0;
	}
	else if(len > len2)
		return -1;
	else if(len < len2)
		return 1;
	else {
		Log("Comparison error", ERROR, STRING);
		return 0;
	}
}
/**
 * Porovna string s charovym polem
 */
int compareString(struct String* s, char* s2){
	if(s == NULL || s2 == NULL){
		Log("Empty params", ERROR, STRING);
		global.errno = intern;
		return 0;
	}
	int charlen = getCharArrayLength(s2);
	if(s->Length == charlen){
		// jsou stejne delky, zacit porovnavat po znacich
		for(int i = 0; i < s->Length; i++){
			if(s->Value[i] != s2[i]){
				if(s->Value[i] < s2[i])
					return -1;
				else 
					return 1;
			}			
		}
		
		return 0;
	}
	else if(s->Length > charlen)
		return -1;
	else if(s->Length < charlen)
		return 1;
	else {
		Log("Comparison error", ERROR, STRING);
		return 0;
	}
}
/**
 * Porovnava obsahy dvou Stringovych struktur.
 */
int compareStrings(struct String* s1, struct String* s2){
	if(s1 == NULL || s2 == NULL){
		Log("s1 or s2 = null", ERROR, STRING);
		global.errno = intern;
		return 0;	
	}
	else {
		fprintf(stderr, "%s ? %s\n", s1->Value, s2->Value);
		if(s1->Length == s2->Length){
			// maji stejnou delku

			for(int i = 0; i < s1->Length; i++){
				if(s1->Value[i] != s2->Value[i]){
					if(s1->Value[i] < s2->Value[i])
						return -1;
					else 
						return 1;
				}
			}
		
			return 0;
		}
		else if(s1->Length > s2->Length)
			return -1;
		else if(s1->Length < s2->Length)
			return 1;
	}
	
	Log("Comparison error", ERROR, STRING);
	return 0;
}

/**
 * Vraci delku pole charu, pro pocitani delky.
 */
int getCharArrayLength(char* array){
	char cur = ' ';
	int i = 0;

	while((cur = array[i]) != '\0'){
		i++;
	}

	return i;
}

/**
 * Posune vsechny uppercase znaky do lowercase
 */
int toLower(struct String* str){
	for(int i = 0; i < str->Length; i++){
		if(str->Value[i] >= 'A' && str->Value[i] <= 'Z')
			str->Value[i] += TOLOWER;
	}
	
	return True;
}

/**
 * Skopiruje data ze stringu do druheho
 */
int copyString(struct String* src, struct String** dest){
	if(src == NULL){
		Log("Src string == empty", ERROR, STRING);
		global.errno = intern;
		return False;	
	}
	
	if((*dest) == NULL && ((*dest) = makeNewString()) == NULL){
		Log("Destination string cannot be maked", ERROR, STRING);
		return False;
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

/**
 * Skopiruje data z src pole do struktury dest
 */
int copyFromArray(char* src, struct String** dest){
	if(src == NULL){
		Log("Src string == empty", ERROR, STRING);
		global.errno = intern;
		return False;	
	}
	if((*dest) == NULL && ((*dest) = makeNewString()) == NULL)
		return False;
	else {
		if(!emptyString(*dest)){
			Log("Clearing failed", ERROR, STRING);
			return False;
		}
	}
		
	char c = ' ';
	int i = 0;
	while((c = src[i++]) != '\0'){
		if(!addChar((*dest), c))
			return False;
	}
	
	return True;
}
