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
	if(!global.src)
	{
		fprintf(stderr, "Failed to open a file %s\n", srcpath);
	}
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
	if(argc < 2){
		Log("Spatne parametry..", ERROR, MAIN);
		return False;
	}
	
	if(init(argv[1]) != True){
		Log("main: main_all struct dont allocated garbage collector..", ERROR, MAIN);
		return False;
	}
	
	struct toc *tmp;
	

	if(!compareCharArrays(argv[2], "--stack")){
		// spusteni cteni - lexikalni anal-lyzator
		struct stack* stack = makeNewStack();
		struct String* str = makeNewString();
		while(!feof(global.src)){
			c = fgetc(global.src);
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
					struct symbolTableNode* node = insertValue(&symtable, str, DT_INT);
					if(node == NULL){
						break;					
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
	else if(!compareCharArrays(argv[2], "--run")){
		// pokud vse probehlo OK, tak zobrazit strom
		if(parser()){
			printAst(global.program);	
			
			Log("Printing symbol table top layer", DEBUG, MAIN);
			struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
			printSymbolTable(top, 0);	
			Log("Printing function table", DEBUG, MAIN);
			printSymbolTable(global.funcTable, 0);
		}	
	}
	
	atexit(quit);
	return global.errno;
}
