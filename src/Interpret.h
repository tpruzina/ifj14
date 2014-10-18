#ifndef _INTERPRET_H
#define _INTERPRET_H

#include <stdio.h>
#include <stdlib.h>
#include "GC.h"
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "Ast.h"
#include "ial.h"
#include "Scanner.h"
#include "Parser.h"

int interpret();

#ifdef __GNUC__
void *runTree(struct astNode *restrict curr) __attribute__ ((fastcall));
#endif

#ifndef __GNUC__
void *runTree(struct astNode *curr);
#endif


#endif
