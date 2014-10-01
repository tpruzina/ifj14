#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Defines.h"
#include "Structs.h"
#include "String.h"

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
void skipWSandComments();

// pomocna funkcia, das jej typ a navrati string
// napriklad T_NOT vrati "not"

const char *returnTypeAsStr(int type);


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

	KA_STR_LIT,
	KA_STR_LIT_INISDE,
	KA_STR_LIT_DONE,
	KA_SHARP,			// #

	KA_EQ,	// =

	KA_LESS,	// <
	KA_NEQV,	// <>
};

#endif /* __SCANNER_H__ */


