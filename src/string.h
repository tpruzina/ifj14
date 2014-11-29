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

#ifndef _STRINGH_
#define _STRINGH_

#include "structs.h"
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
