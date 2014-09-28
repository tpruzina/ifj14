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
#include "Defines.h"

const int TOLOWER = (int)('a' - 'A');

/**
 * Vytvari novou strukturu a vraci ji.
 * TODO: po udelani GC, prepsat malloc na gcmalloc
 */
struct String* makeNewString(struct main_all**ma){
	struct String* str = (struct String*)gcMalloc(ma, sizeof(struct String));
	if(str == NULL){
		LogError("String: makeNewString: malloc error");
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
int addChar(struct main_all**ma, struct String* s, char c){
	if(s == NULL){
		// v pripade, ze dostane prazdny string -> zalozi novy a prida znak
		s = makeNewString(&(*ma));
		LogDebug("String: addChar: novy string");
	}
	
	if(s->Value == NULL){
		// v pripade, ze string sice alokovan byl, ale neobsahoval hodnotu
		// -> alokovat pole a vlozit znak + \0 nakonec
		s->Value = (char*)gcMalloc(ma, sizeof(char)*2);

		s->Value[0] = c;
		s->Value[1] = '\0';
		
		s->Allocated = 2;
		s->Length = 1;

		//LogDebug("String: addChar: pridani znaku (alokace)");
		return true;
	}
	else {
		// v pripade, ze se pouze pridava znak do pole
		s->Value = (char*)gcRealloc(ma, s->Value, s->Allocated + 2);
		s->Value[s->Length] = c;
		s->Value[s->Length+1] = '\0';
		
		s->Allocated += 2;
		s->Length += 1;

		//LogDebug("String: addChar: pridani znaku (realokace)");
		return true;
	}

	LogWarning("String: addChar: neco probehlo spatne");
	return false;
}

int emptyString(struct main_all** ma, struct String* str){
	if(str == NULL)
		return false;
	
	str->Value = (char*)gcRealloc(ma, str->Value, 1);
	str->Value[0] = '\0';
	str->Allocated = 1;
	str->Length = 0;
	return true;
}

/**
 * Tiskne obsah struktury Stringu
 */
int printString(struct String* s){
	if(s == NULL){
		return false;
	}
	printf("String:\n");
	printf("   Value:    \t\"%s\"\n", s->Value);
	printf("   Length:   \t%d\n", s->Length);
	printf("   Allocated:\t%d\n", s->Allocated);
	
	return false;
}

/**
 * Uvolni vytvoreny string, vcetne odepsani z GC.
 * Kontroly na spravne predany string.
 */
int freeString(struct main_all** ma, struct String* s){
	if(s == NULL){
		LogWarning("String: freeString: predany string je prazdny");
		return false;
	}

	gcFree(ma, s);
	return true;
}

/**
 * Porovna string s charovym polem
 * V pripade shody vraci True (1)
 * v pripade neshody vraci false (0)
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
				// jedna neshoda = nejsou stejne
				return false;
			}			
		}
		
		return true;
	}
	return false;
}
/**
 * Porovnava obsahy dvou Stringovych struktur.
 * V pripade shody: True
 * V pripade neshody: false
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
				// neshoda
				return false;
			}
		}
		
		return true;
	}
	return false;
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
	
	return true;
}
