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
#include "Interpret.h"


struct mainAll global;

/*
 * Alokuje a nastavi hlavni strukturu. 
 * V pripade chyby alokace vraci false.
 *
 * struct main_all** ma -- odkaz na strukturu, obsahujici data
 * char* srcpath		-- cesta k souboru, ktery ma byt interpretovan
 */
int init(char* srcpath){

	global.gc = malloc(sizeof(struct gc));
	if(global.gc == NULL){
		global.errno = intern;
		return False;
	}

	global.gc->list = NULL;
	global.gc->listLength = 0;

	global.src = fopen(srcpath, "r");
	global.errno = ok;
	global.lineCounter = 0;
	
	global.program = NULL;
	global.symTable = makeNewStack();	
	
	Log("main:init - done", DEBUG, MAIN);
	return True;
}
/*
 * Uvolni vsechny alokovane prostredky.
 *
 * struct main_all** ma -- odkaz na strukturu, obsahujici data
 */
void quit(){
	//gcPrintState();
	gcFreeAll();
	free(global.gc);
	fclose(global.src);
	Log("main:exit - done", DEBUG, MAIN);
}

int main(int argc, char** argv)
{


	if(argc == 2)
	{
		if(init(argv[1]) != True){
			Log("main: main_all struct dont allocated garbage collector..", ERROR, MAIN);
			exit(intern);
		}
		atexit(quit);
		// pokud vse probehlo OK, tak zobrazit strom
		if(parser())
		{
#ifdef _DEBUG
			Log("Printing PROGRAM",DEBUG,MAIN);
			printAst(global.program);
			fflush(stderr);
			fflush(stdout);
			Log("Printing PROGRAM",DEBUG,MAIN);

			Log("Printing symbol table top layer", DEBUG, MAIN);
			struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
			printSymbolTable(top, 0);
			Log("Printing function table", DEBUG, MAIN);
			printSymbolTable(global.funcTable, 0);
#endif

			interpret();
#ifdef _DEBUG
			printSymbolTable(top, 0);
#endif
		}
	}
	return global.errno;
}
