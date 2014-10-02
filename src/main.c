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
	
	//struct mainAll global = {0};
	
	if(init(argv[1]) != True){
		Log("main: main_all struct dont allocated garbage collector..", ERROR, MAIN);
		return False;
	}
	

	if(!compareCharArrays(argv[2], "--stack")){
		// spusteni cteni - lexikalni anal-lyzator
		struct stack* stack = makeNewStack();
		struct String* str = makeNewString();
		char c = ' ';
		while(!feof(global.src)){
			c = fgetc(global.src);

			if(c == ' ' || c == '\n' || c == '\r' || c == '\t'){
				// separator - konec tokenu
				if(str->Length != 0){
					//printString(str);
					//emptyString(&ma, str);
				
					if(stackPush(stack, str)){
						Log("Value pushed", DEBUG, MAIN);
						printString(str);
					}

					str = makeNewString();
				}
			}
			else
				addChar(str, c);
		}	
		Log("READING....", DEBUG, MAIN);
		while(!stackEmpty(stack)){
			struct String* s = (struct String*)stackPop(stack);
			printString(s);	
		}
		Log("END", DEBUG, MAIN);
	}
	else if(!compareCharArrays(argv[2], "--ast")){
		struct astNode* ast = makeNewAST();
		ast->type = AST_START;
		struct astNode* l = makeNewAST();
		l->type = AST_ADD;
		struct astNode* r = makeNewAST();
		r->type = AST_IF;
		struct astNode* l1 = makeNewAST();
		l1->type = AST_NUM;
		struct astNode* r1 = makeNewAST();
		r1->type = AST_NUM;
		l->left = l1;
		l->right = r1;
		ast->left = l;
		ast->right = r;
	
		printAst(ast);
	}
	else if(!compareCharArrays(argv[2], "--symbol")){
		// demonstrace tabulky symbolu
		
		struct symbolTableNode* symtable = NULL;
		
		struct String* str = makeNewString();
		char c = ' ';
		while(!feof(global.src)){
			c = fgetc(global.src);

			if(c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == ';' || c == '.' || c == ',' || c == ':' || c == '{' || c == '}'){
				// separator - konec tokenu
				if(str->Length != 0){
					printString(str);
					struct symbolTableNode* node = insertValue(&symtable, str, NULL);
					if(node == NULL){
						break;					
					}
					else {
						// vkladat hodnoty
											
					}
				
					//printString(str);
					str = makeNewString();
				}
			}
			else
				addChar(str, c);
		}
		if(global.errno == 0){
			printSymbolTable(symtable, 0);
		
			struct symbolTableNode* copy;
			copyTable(symtable, &copy);
			printSymbolTable(copy, 0);
		
			if(!deleteTable(&copy)){
				Log("Deleting error", ERROR, MAIN);
			}
			printSymbolTable(copy, 0);
		}
	}
	else if(!compareCharArrays(argv[2], "--expr")){
		Log("Starting parsing", WARNING, MAIN);
		struct astNode* ast = parseException();
		if(ast == NULL)
			Log("Parser failed", WARNING, MAIN);
		else
			Log("Parser successed", WARNING, MAIN);
			
		printAst(ast);
	}
	
	
	fprintf(stderr, "Error code: %d\n", (int)global.errno);
	
	atexit(quit);
	return global.errno;
}
