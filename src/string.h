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
struct main_all;

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
struct String* makeNewString(struct main_all**);
int addChar(struct main_all**,struct String*, char);
int emptyString(struct main_all**, struct String*);
int printString(struct String*);
int freeString(struct main_all**,struct String*);
int toLower(struct String*);
int getCharArrayLength(char*);
int compareString(struct String*, char*);
int compareStrings(struct String*, struct String*);
#endif
