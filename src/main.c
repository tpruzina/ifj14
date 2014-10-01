#include <stdio.h>
#include <stdlib.h>
#include "GC.h"
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "Ast.h"
#include "SymbolTable.h"
#include "String.h"
#include "Scanner.h"

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
	if(!global.src)
	{
		fprintf(stderr, "Failed to open a file %s\n", srcpath);
	}
	global.errno = ok;
	
	Log("main:init - done", DEBUG, MAIN);
	return True;
}
/*
 * Uvolni vsechny alokovane prostredky.
 *
 * struct main_all** ma -- odkaz na strukturu, obsahujici data
 */
void quit(){
	//gcPrintState(ma);
	gcFreeAll();
	free(global.gc);
	fclose(global.src);
	Log("main:exit - done", DEBUG, MAIN);
}

int main(int argc, char** argv)
{
	if(argc < 2){
		Log("Spatne parametry..", ERROR, MAIN);
		return False;
	}
	
	if(init(argv[1]) != True){
		Log("main: main_all struct dont allocated garbage collector..", ERROR, MAIN);
		return False;
	}
	
	if(!global.src)
		return -99;

	struct toc *tmp;

	const char *str;
	while(true)
	{
		tmp = getToc();
		str = returnTypeAsStr(tmp->type);

		if(str)
		{
			if(tmp->type == T_INT)
				printf("%s(%d)\n",str,tmp->data.integer);
			else if(tmp->type == T_REAL)
				printf("%s(%f)\n",str,tmp->data.real);
			else if(tmp->type == T_STR)
				printf("%s(%s)\n",str,tmp->data.str->Value);
			else
				printf("%s\n",str);
		}
		else
			printf("%d\n",tmp->type);
		fflush(stdout);
		fflush(stderr);


		if(tmp->type == T_EOF)
			break;
	}
	
	atexit(quit);
	return 0;
}
