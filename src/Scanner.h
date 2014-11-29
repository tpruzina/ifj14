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

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Defines.h"
#include "Structs.h"
#include "String.h"
#include "GC.h"

struct toc
{
//	int type;
	enum tokenType type;
	union
	{
		struct String *str;
		double	real;
		int	integer;
		bool	boolean;
	} data;
};

struct toc * getToc();

void tocInit(struct toc **);


// pomocna funkcia, das jej typ a navrati string
// napriklad T_NOT vrati "not"
const char *returnTypeAsStr(int type);

// pomocne funkcie ktore taktiez pocitaju pocet '\n'
int getChar();
void unGetChar(char);

// pomocne funkcie scannera
int ascii(unsigned char);
void parse_escape_seq(int *c);
void skipWSandComments();	

enum KA_STATES
{
	KA_ERR = -1,
	KA_START,
	
	KA_INTEGER,
	KA_INT_DOT,
	KA_REAL,
	KA_REAL_EXP,
	KA_REAL_EXP_PM,
	KA_REAL_DOT_EXP_PM,
	KA_REAL_EXP_NUM,

	KA_IDENT,
	
	KA_COL,
	KA_ASGN,

	KA_DOT,
	KA_SCOL,
	KA_COM,

	KA_STR_LIT,
	KA_STR_LIT_INISDE,
	KA_STR_LIT_DONE,
	//here the fun begins
	KA_SHARP,			// #

	KA_EQ,		// =
	KA_GRT,		// >
	KA_GEQV,	// >=
	KA_LSS,	// <
	KA_LEQV,	// <=
	KA_NEQV,	// <>

	KA_ADD,		// +
	KA_SUB,		// -
	KA_MUL,		// *
	KA_DIV,		// /

	KA_LPAR,
	KA_RPAR,
	KA_LBRC,
	KA_RBRC,
	KA_LCBR,
	KA_RCBR,
};

#endif /* __SCANNER_H__ */


