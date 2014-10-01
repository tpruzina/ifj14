#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Defines.h"
#include "Structs.h"
#include "String.h"
#include "GC.h"

struct toc
{
	int type;
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
//preskoci whitespace a komentare
void skipWSandComments();

// pomocna funkcia, das jej typ a navrati string
// napriklad T_NOT vrati "not"
const char *returnTypeAsStr(int type);

// pomocna premenna pre stringove literaly
int asci(unsigned char);


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
	KA_SHARP,// #
	KA_S_0,				// #0, #00 atp
	KA_S_1,				// #1
	KA_S_2,				// #2
	KA_S_39,			// #[3-9]
	KA_S_239_69N,	//slouceny stav - ak dostaneme cislicu tak budeme >255
	KA_S_12_04N,	//slouceny stav	- sme < 249
	KA_S_2_5,		// mame #25	- ak dostaneme > 6 error (226-229) inak (250-255)

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


