#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "GC.h"
#include "Structs.h"
#include "Log.h"
#include "Stack.h"
#include "Ast.h"

/*
 * Alokuje a nastavi hlavni strukturu. 
 * V pripade chyby alokace vraci false.
 *
 * struct main_all** ma -- odkaz na strukturu, obsahujici data
 * char* srcpath		-- cesta k souboru, ktery ma byt interpretovan
 */
int init(struct mainAll** ma, char* srcpath){
	*ma = malloc(sizeof(struct mainAll));
	if((*ma) == NULL)
		return False;
		
	(*ma)->gc = malloc(sizeof(struct gc));
	if((*ma)->gc == NULL){
		(*ma)->errno = intern;
		return False;
	}

	(*ma)->gc->list = NULL;
	(*ma)->gc->listLength = 0;

	(*ma)->src = fopen(srcpath, "r");
	(*ma)->errno = ok;
	
	Log("main:init - done", DEBUG, MAIN);
	return True;
}
/*
 * Uvolni vsechny alokovane prostredky.
 *
 * struct main_all** ma -- odkaz na strukturu, obsahujici data
 */
void quit(struct mainAll** ma){
	gcPrintState(ma);
	gcFreeAll(ma);
	free((*ma)->gc);
	free((*ma));
	
	Log("main:exit - done", DEBUG, MAIN);
}

/*
 * 
 *
 *
 */
int main(int argc, char** argv)
{
	if(argc < 2){
		Log("Spatne parametry..", ERROR, MAIN);
		return False;
	}
		
	struct mainAll* ma;
	if(init(&ma, argv[1]) != True){
		if(ma == NULL) {
			Log("main: main_all struct is not allocated", ERROR, MAIN);
			return False;
		}
		else if(ma != NULL && ma->gc == NULL){
			Log("main: main_all struct dont allocated garbage collector..", ERROR, MAIN);
			return False;
		}
	}

	if(compareCharArrays(argv[2], "--stack")){
		// spusteni cteni - lexikalni anal-lyzator
		struct stack* stack = makeNewStack(&ma);
		struct String* str = makeNewString(&ma);
		char c = ' ';
		while(!feof(ma->src)){
			c = fgetc(ma->src);

			if(c == ' ' || c == '\n' || c == '\r' || c == '\t'){
				// separator - konec tokenu
				if(str->Length != 0){
					//printString(str);
					//emptyString(&ma, str);
				
					if(stackPush(&ma, stack, str)){
						Log("Value pushed", DEBUG, MAIN);
						printString(str);
					}

					str = makeNewString(&ma);
				}
			}
			else
				addChar(&ma, str, c);
		}	
		Log("READING....", DEBUG, MAIN);
		while(!stackEmpty(&ma, stack)){
			struct String* s = (struct String*)stackPop(&ma, stack);
			printString(s);	
		}
		Log("END", DEBUG, MAIN);
	}
	else if(compareCharArrays(argv[2], "--ast")){
		struct astNode* ast = makeNewAST(&ma);
		ast->type = AST_START;
		struct astNode* l = makeNewAST(&ma);
		l->type = AST_ADD;
		struct astNode* r = makeNewAST(&ma);
		r->type = AST_IF;
		struct astNode* l1 = makeNewAST(&ma);
		l1->type = AST_NUM;
		struct astNode* r1 = makeNewAST(&ma);
		r1->type = AST_NUM;
		l->left = l1;
		l->right = r1;
		ast->left = l;
		ast->right = r;
	
		printAst(ast);
	}
	else if(compareCharArrays(argv[2], "--symbol")){
		// demonstrace tabulky symbolu
		
		
	}
	
	// uvolneni prostredku
	fclose(ma->src);
	int errno = ma->errno;
	quit(&ma);	
	return errno;
}
