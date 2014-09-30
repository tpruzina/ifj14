/*
 * String.h
 * 
 * Hlavickovy soubor knihovny String.
 * Obsahuje definici struktury String a definice funkci pro praci s ni.
 * 
 */
#ifndef _STRINGH_
#define _STRINGH_

#include "Structs.h"
struct mainAll;

/**
 * Struktura stringu
 * Value 		- obsahuje text v podobe pole charu
 * Length 		- obsahuje delku textu
 * Allocated 	- obsahuje pocet alokovanych bytu
 */
struct String {
	char* Value;
	int Length;
	int Allocated;
};

/*
 *	Definice funkci knihovny String
 */
struct String* makeNewString();
int addChar(struct String*, char);
int emptyString(struct String*);
int printString(struct String*);
int freeString(struct String*);
int toLower(struct String*);
int getCharArrayLength(char*);
int compareCharArrays(char*, char*);
int compareString(struct String*, char*);
int compareStrings(struct String*, struct String*);
int copyString(struct String*, struct String**);
int copyFromArray(char*, struct String**);
#endif
