#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifdef _DEBUG
#include <errno.h>
#include <assert.h>
#endif

#include <stdio.h>		// FILE
#include <stdbool.h>		// true/false

struct global {
	FILE *fd;
};

// typy return kodov, chyby su v osobitom enume
// zadani 2,0
enum RET_TYPES {
	RET_OK = 0,
	RET_ERR_LEX = 1,
	RET_ERR_SYNT = 2,
	RET_ERR_SEM_UNDEF_REDEF = 3,
	RET_ERR_SEM_COMPAT = 4,
	RET_ERR_SEM_OTHERS = 5,
	RET_ERR_NUMERIC_INPUT = 6,
	RET_ERR_UNDEFINED_VAR = 7,
	RET_ERR_DIVBYZERO = 8,
	RET_ERR_RUNTIME_OTHER = 9,
	RET_ERR_INTERNAL = 99
};

#endif				/* __DEFINES_H__ */
