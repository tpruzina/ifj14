#ifndef _INTERPRET_H
#define _INTERPRET_H

#include <stdio.h>
#include <stdlib.h>
#include "GC.h"
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "Ast.h"
#include "SymbolTable.h"
#include "Scanner.h"
#include "Parser.h"

int interpret();

#endif
