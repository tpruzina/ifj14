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
 * TODO: po udelani GC, prepsat malloc na gcmalloc
 */
struct String* makeNewString(struct mainAll**ma){
	struct String* str = (struct String*)gcMalloc(ma, sizeof(struct String));
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
int addChar(struct mainAll**ma, struct String* s, char c){
	if(s == NULL){
		// v pripade, ze dostane prazdny string -> zalozi novy a prida znak
		s = makeNewString(&(*ma));
		Log("String: addChar: novy string", DEBUG, STRING);
	}
	
	if(s->Value == NULL){
		// v pripade, ze string sice alokovan byl, ale neobsahoval hodnotu
		// -> alokovat pole a vlozit znak + \0 nakonec
		s->Value = (char*)gcMalloc(ma, sizeof(char)*2);

		s->Value[0] = c;
		s->Value[1] = '\0';
		
		s->Allocated = 2;
		s->Length = 1;

		//Log("String: addChar: pridani znaku (alokace)", DEBUG, STRING);
		return True;
	}
	else {
		// v pripade, ze se pouze pridava znak do pole
		s->Value = (char*)gcRealloc(ma, s->Value, s->Allocated + 1);
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

int emptyString(struct mainAll** ma, struct String* str){
	if(str == NULL)
		return False;
	
	str->Value = (char*)gcRealloc(ma, str->Value, 1);
	str->Value[0] = '\0';
	str->Allocated = 1;
	str->Length = 0;
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
int freeString(struct mainAll** ma, struct String* s){
	if(s == NULL){
		Log("String: freeString: predany string je prazdny", WARNING, STRING);
		return False;
	}

	gcFree(ma, s);
	return True;
}

int compareCharArrays(char* s1, char* s2){
	if(s1 == NULL || s2 == NULL)
		return False;
	
	int len = getCharArrayLength(s1);
	if(len == getCharArrayLength(s2)){
		for(int i = 0; i < len; i++){
			if(s1[i] != s2[i]){
				if(s1[i] < s2[i])
					return -1;
				else
					return 1;			
			}
		}
		return True;
	}
	else {
		return False;
	}
}
/**
 * Porovna string s charovym polem
 * V pripade shody vraci True (1)
 * v pripade neshody vraci False (0)
 * v pripade chyby:
 * 		-1 = predana struktura je null
 * 		-2 = predane pole charu je null
 */
int compareString(struct String* s, char* s2){
	if(s == NULL)
		return -1;
	if(s2 == NULL)
		return -2;

	if(s->Length == getCharArrayLength(s2)){
		// jsou stejne delky, zacit porovnavat po znacich

		for(int i = 0; i < s->Length; i++){
			if(s->Value[i] != s2[i]){
				if(s->Value[i] < s2[i])
					return -1;
				else 
					return 1;
				// jedna neshoda = nejsou stejne
				return False;
			}			
		}
		
		return True;
	}
	return False;
}
/**
 * Porovnava obsahy dvou Stringovych struktur.
 * V pripade shody: True
<<<<<<< HEAD
 * V pripade neshody: false
=======
 * V pripade neshody: False
>>>>>>> master
 * V pripade chyb:
 * 		-1 = jedna nebo druha struktura je null
 */
int compareStrings(struct String* s1, struct String* s2){
	if(s1 == NULL || s2 == NULL)
		return -1;

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
		
		return True;
	}
	return False;
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
int copyString(struct mainAll** ma, struct String* src, struct String** dest){
	if((*ma) == NULL){
		Log("Main all empty", ERROR, STRING);
		return False;
	}
	
	if(src == NULL){
		Log("Src string == empty", ERROR, STRING);
		(*ma)->errno = intern;
		return False;	
	}
	
	if((*dest) == NULL && ((*dest) = makeNewString(ma)) == NULL)
		return False;
	else {
		if(!emptyString(ma, *dest)){
			return False;
		}
	}

	for(int i = 0; i < src->Length; i++){
		if(!addChar(ma, (*dest), src->Value[i])){
			return False;
		}
	}
	
	return True;
}
int copyFromArray(struct mainAll** ma, char* src, struct String** dest){
	if((*ma) == NULL){
		Log("Main all empty", ERROR, STRING);
		return False;
	}
	
	if(src == NULL){
		Log("Src string == empty", ERROR, STRING);
		(*ma)->errno = intern;
		return False;	
	}
	
	if((*dest) == NULL && ((*dest) = makeNewString(ma)) == NULL)
		return False;
	else {
		if(!emptyString(ma, *dest)){
			return False;
		}
	}
		
	char c = ' ';
	int i = 0;
	while((c = src[i++]) != '\0'){
		if(!addChar(ma, (*dest), c))
			return False;
	}
	
	return False;
}
