#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Stack.h"
#include "Log.h"
#include "Ast.h"
#include "Scanner.h"
#include "Parser.h"
#include "SymbolTable.h"


/**
 * Interni funkce parseru, podle tokenu vrati jeho prioritu.
 * Vaze se hlavne na operatory u vsech ostatnich vraci prioritu 10
 * --------------------
 * t: token k zjisteni priority
 */
int getPriority(struct toc* t){
	if(t == NULL){
		return 0;
	}

	switch(t->type){
		case T_EQV:
		case T_NEQV:
		case T_GRT:
		case T_LSS:
		case T_GEQV:
		case T_LEQV:
			return 1;
		case T_ADD:
		case T_SUB:
		case T_OR:
		case T_XOR:
			return 2;
		case T_MUL:
		case T_DIV:
		case T_AND:
			return 3;
		case T_NOT:
			return 4;
		default:
			return 0;	
	}
}

/**
 * Prevadi Token type na AST node type. Pri vytvareni stromu vyrazu to pomuze.
 * --------------------
 * t: Token u ktereho je potreba zjistit typ.
 */
int convertTokenToNode(struct toc* t){
	if(t == NULL){
		return -1;
	}
	
	switch(t->type){
		case T_EQV:
			return AST_EQV;
		case T_NEQV:
			return AST_NEQV;
		case T_GRT:
			return AST_GRT;
		case T_LSS:
			return AST_LSS;
		case T_GEQV:
			return AST_GEQV;
		case T_LEQV:
			return AST_LEQV;
		case T_ADD:
			return AST_ADD;
		case T_SUB:
			return AST_SUB;
		case T_OR:
			return AST_OR;
		case T_XOR:
			return AST_XOR;
		case T_MUL:
			return AST_MUL;
		case T_DIV:
			return AST_DIV;
		case T_AND:
			return AST_AND;
		case T_NOT:
			return AST_NOT;
		case T_NMB:
			return AST_NUM;
		case T_TEXT:
			return AST_STR;
		case T_ID:
			return AST_ID;	
		
		default:
			return -1;
	}
}

struct tokennames {
	char* str;
	int type;
};
struct tokennames tokennames[] = {
	{"T_KW_BEGIN", T_KW_BEGIN},
	{"T_KW_BOOLEAN",T_KW_BOOLEAN},
	{"T_KW_DO", T_KW_DO},
	{"T_KW_ELSE", T_KW_ELSE},
	{"T_KW_END", T_KW_END},
	{"T_KW_ENDDOT", T_KW_ENDDOT},
	{"T_KW_FALSE", T_KW_FALSE},
	{"T_KW_FALSE", T_KW_FALSE},
	{"T_KW_FORW", T_KW_FORW},
	{"T_KW_FUNC",T_KW_FUNC},
	{"T_KW_IF", T_KW_IF},
	{"T_KW_INT", T_KW_INT},
	{"T_KW_READLN",T_KW_READLN},
	{"T_KW_REAL", T_KW_REAL},
	{"T_KW_SORT",T_KW_SORT},
	{"T_KW_STRING", T_KW_STRING},
	{"T_KW_THEN", T_KW_THEN},
	{"T_KW_TRUE", T_KW_TRUE},
	{"T_KW_VAR", T_KW_VAR},
	{"T_KW_WHILE",T_KW_WHILE},
	{"T_KW_WRT", T_KW_WRT},
	{"T_KW_PROGRAM",T_KW_PROGRAM},
	{"T_RPT",T_RPT},
	{"T_UNTIL",T_UNTIL},
	{"T_ID",T_ID},
	{"T_TEXT",T_TEXT},
	{"T_NMB",T_NMB},
	{"T_INT", T_INT},
	{"T_REAL", T_REAL},
	{"T_STR", T_STR},
	{"T_BOOL", T_BOOL},
	{"T_ARR", T_ARR},
	{"T_OF", T_OF},
	{"T_ASGN", T_ASGN},
	{"T_EQV", T_EQV},
	{"T_NEQV", T_NEQV},
	{"T_GRT", T_GRT},
	{"T_LSS", T_LSS},
	{"T_GEQV", T_GEQV},
	{"T_LEQV", T_LEQV},
	{"T_ADD", T_ADD},
	{"T_SUB", T_SUB},
	{"T_MUL", T_MUL},
	{"T_DIV", T_DIV},
	{"T_AND", T_AND},
	{"T_OR", T_OR},
	{"T_XOR", T_XOR},
	{"T_NOT", T_NOT},
	{"T_SCOL", T_SCOL},
	{"T_COL", T_COL},
	{"T_DOT", T_DOT},
	{"T_COM", T_COM},
	{"T_APS", T_APS},
	{"T_LPAR", T_LPAR},
	{"T_RPAR", T_RPAR},
	{"T_LBRC", T_LBRC},
	{"T_RBRC", T_RBRC},
	{"T_LCBR", T_LCBR},
	{"T_RCBR", T_RCBR},
	{"T_DDOT", T_DDOT},
	{"T_USC", T_USC},
	{"T_EOF", T_EOF},
	{NULL, 0}};
	
void printTokenType(struct toc* token){
	for(int i=0; tokennames[i].str; i++)
		if(token->type == tokennames[i].type){
			fprintf(stderr, "Token: %s\n", tokennames[i].str);
			
			if(token->type == T_ID){
				fprintf(stderr, "\t%s\n", token->data.str->Value);
			}
		}
}


void printAstStack(struct stack* aststack){
	fprintf(stderr, "AST STACK..\n");
	
	struct stackItem* item = aststack->Top;
	
	while(item != NULL){
		printAst((struct astNode*)item->Value);
		item = item->Next;
		if(item != NULL)
			fprintf(stderr, "------------------------------\n");	
	}	
	fprintf(stderr, "AST STACK END\n");
}

/**
 * Vytvari novy uzel do stromu podle operatoru.
 * --------------------
 * token: Urcuje typ jakeho by mel byl uzel AST
 * aststack: Urcuje zasobnik ze ktereho bude tahat data a kam bude ukladat vysledek
 */
int makeAstFromToken(struct toc* token, struct stack** aststack){
	fprintf(stderr, "MAFT\t");
	printTokenType(token);
	printAstStack(*aststack);

	struct astNode* node = makeNewAST();
	// vytazeny operator
	int prio = getPriority(token);
	if(prio > 0 && prio < 10){
		// operatory bez NOT
		node->type = convertTokenToNode(token);
		if((int)node->type == -1){
			Log("Unsuported token type to convert", ERROR, PARSER);
			return False;
		}
		
		node->right = (struct astNode*)stackPop(*aststack);
		node->left = (struct astNode*)stackPop(*aststack);
	}
	else if(prio == 0){
		// not
		node->type = AST_NOT;
		node->left = (struct astNode*)stackPop(*aststack);
	}
	else {
		Log("Stack error - invalid data", WARNING, PARSER);
		printTokenType(token);
		return False;						
	}
	
	
	printAst((struct astNode*)stackTop(*aststack));
	return stackPush(*aststack, node);
}










/**
 * Hlavni funkce parseru - spousti rekurzivni pruchod
 * --------------------
 */
int parser(){
	struct astNode* prog = parseProgram();
	if(!prog)
		return False;
	
	global.program = prog;
	return True;
}

/**
 * Kontroluje zacatek programu, existenci tokenu program, jmeno programu a nasledne telo
 * -------------------------------------------------------------------------------------
 */
struct astNode* parseProgram(){
	Log("parseProgram", WARNING, PARSER);
	struct toc* cur = getToc();

	if(cur->type != T_KW_PROGRAM){
		Log("Syntax error - expected PROGRAM keyword", ERROR, PARSER);
		global.errno = synt;
		return NULL;
	}
	
	// nalezen start
	cur = getToc();
	if(cur->type != T_ID){
		// chyba, melo nasledovat oznaceni
		Log("Syntax error - expected name of program", ERROR, PARSER);
		exit(synt);
	}
	
	// jmeno programu
	struct astNode* program = makeNewAST();
	program->type = AST_START;
	// kopirovani jmena programu
	if(!(copyString(cur->data.str, &(program->data.str))))
		return NULL;			
	printString(program->data.str);
	
	cur = getToc();
	if(cur->type != T_SCOL){
		Log("Syntax error - expected SEMICOLON", ERROR, PARSER);
		exit(synt);
	}
					
	// ok -> nasleduje telo programu nebo definice promennych nebo definice funkci/dopredne definice funkci			
	// pokud bude nasledujici token BEGIN pak ctu telo
	// pokud bude nasledujici VAR pak jsou to lokalni 
	struct astNode* vardef = parseVars(&cur);
	
	// muze vratit null, ale jen pokud bude nastaveny chybovy kod, jedna se o chybu
	if(vardef == NULL && global.errno != ok){
		// vyskytla se chyba
		return NULL;				
	}
	Log("program: back from VARS", DEBUG, PARSER);
	// lokalni promenne
	program->other = vardef;
		
	// prochazet definice funkci a vkladat je s telem do tabulky symbolu
	while(cur->type == T_KW_FUNC){
		struct astNode* func = parseFunction();				
		if(!func)
			return NULL;
		
		// muze nacitat 
		cur = getToc();				
	}
	
	printTokenType(cur);
	
	// nasleduje telo programu
	struct astNode* body = parseBody(&cur);		
	if(!body)
		return NULL;
	program->left = body;
	
	cur = getToc();
	if(cur->type != T_EOF){
		Log("program: Syntax error - expected program end", ERROR, PARSER);
		printTokenType(cur);
		exit(synt);
	}
			
	return program;
}

/**
 * Parsuje telo, pouze zkontroluje zda-li zacina na begin a konci na end.
 * ---------------------------------------------------------------------
 */
struct astNode* parseBody(struct toc** cur){
	Log("parseBody", WARNING, PARSER);
	
	if((*cur)->type != T_KW_BEGIN){ // nebyl nacten BEGIN
		(*cur) = getToc();
		
		if((*cur)->type != T_KW_BEGIN){
			Log("body: Syntax error - expected BEGIN keyword", ERROR, PARSER);
			printTokenType(*cur);		
			exit(synt);
		}
	}
	
	// zacatek tela -> spustit prikazy
	// ukladat je doleva za sebe do typu AST_CMD
	struct astNode* body = makeNewAST();
	struct astNode* cmd = parseCommand(cur);
	if((*cur)->type == T_KW_END){
		// ukonceni tela
		
		if(cmd == NULL){
			Log("body: WARNING - statement without effect", WARNING, PARSER);
		}
		
		body->left = makeNewAST();
		body->left->type = AST_CMD;
		body->right = cmd;
		
		return body;		
	}
	else if((*cur)->type == T_SCOL){	
		// ocekavat dalsi command
		while(cmd != NULL){
			struct astNode* item = body;
			while(item->left != NULL)
				item = item->left;
			item->left = makeNewAST();
			item->left->type = AST_CMD;
			item->left->left = NULL;
			item->left->right = cmd;
		
			// hledat dalsi
			cmd = parseCommand(cur);
			
			// pokud narazi na END tak ukoncit hledani dalsich 
			if((*cur)->type == T_KW_END)
				break;
			else if((*cur)->type == T_SCOL)
				continue;
			else {
				Log("body: Syntax error - unsupported token", ERROR, PARSER);
				printTokenType(*cur);
				exit(synt);			
			}			
		}
	}
	if(!body)
		return NULL;
	return body;
}

/**
 * Parsuje var sekci funkce/programu
 * --------------------
 */
struct astNode* parseVars(struct toc** cur){
	Log("parseVars", WARNING, PARSER);
	(*cur) = getToc();
	if((*cur)->type == T_KW_VAR){
		// zacinat parsovat promenne
		
		struct astNode* node = NULL;
		
		struct symbolTableNode* top = (struct symbolTableNode*)stackPop(global.symTable);
		while(1){
			// definice promennych
			struct astNode* var = makeNewAST();
			(*cur) = getToc();
			if((*cur)->type == T_ID){
				// dostal jsem ID -> ocekavam dvoutecku
			
				if(!(copyString((*cur)->data.str, &(var->data.str))))
					return NULL;
				printString((*cur)->data.str);
			
				// odkladiste pro nove promenne do tabulky symbolu				
				struct symbolTableNode* new = NULL;
				
				(*cur) = getToc();
				if((*cur)->type == T_COL){
					printTokenType(*cur);
					// dostal dvojtecku --> ocekavat typ
														
					(*cur) = getToc();
					switch((*cur)->type){
						case T_KW_INT: {
							var->type = AST_INT;
							// vytvorit novy zaznam v tabulce
							new = insertValue(&top, var->data.str);
							if(!new)
								return NULL;
							
							new->type = DT_INT;													
							break;
						}
						case T_KW_REAL: {
							var->type = AST_REAL;
						
							new = insertValue(&top, var->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_REAL;
							break;
						}
						case T_KW_BOOLEAN: {
							var->type = AST_BOOL;
						
							new = insertValue(&top, var->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_BOOL;
							break;
						}
						case T_KW_STRING: {
							var->type = AST_STR;
							
							new = insertValue(&top, var->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_STR;
							break;
						}
						case T_ARR: {
							var->type = AST_ARR;
						
							struct dataTypeArray* dta = (struct dataTypeArray*)gcMalloc(sizeof(struct dataTypeArray));
							if(!dta)
								return NULL;
							if(!(copyString(var->data.str, &(dta->id))))
								return NULL;
							
							(*cur) = getToc();
							if((*cur)->type == T_LBRC) {
								// leva [ -> ocekavat integer								
								(*cur) = getToc();
								if((*cur)->type != T_INT){
									Log("vars: Syntax error - expected integer as lower range of array index", ERROR, PARSER);
									printTokenType(*cur);
									exit(synt);
								}
								dta->low = (*cur)->data.integer;
								
								// interval - dve tecky mezi integery
								(*cur) = getToc();
								if((*cur)->type != T_DDOT){
									Log("vars: Syntax error - expected two dots", ERROR, PARSER);
									printTokenType(*cur);
									exit(synt);
								}
								
								// ocekavat druhy integer
								(*cur) = getToc();
								if((*cur)->type != T_INT){
									Log("vars: Syntax error - expected integer as higher range of array index", ERROR, PARSER);
									printTokenType(*cur);
									exit(synt);
								}
								dta->high = (*cur)->data.integer;
								
								// ocekavam konec intervalu
								(*cur) = getToc();
								if((*cur)->type != T_RBRC){
									Log("vars: Syntax error - expected right brace", ERROR, PARSER);
									printTokenType(*cur);
									exit(synt);
								}
								
								// ocekavat OF
								(*cur) = getToc();
								if((*cur)->type != T_OF){
									Log("vars: Syntax error - expected OF for defining type of array", ERROR, PARSER);
									printTokenType(*cur);
									exit(synt);
								}
								
								// nacteni typu promenne
								(*cur) = getToc();
								switch((*cur)->type){
									case T_KW_INT: {
										dta->type = DT_INT;
										break;
									}
									case T_KW_REAL: {
										dta->type = DT_REAL;
										break;
									}
									case T_KW_BOOLEAN: {
										dta->type = DT_BOOL;
										break;
									}
									case T_KW_STRING: {
										dta->type = DT_STR;
										break;
									}
									default:
										Log("vars: Syntax error - unsupported type of array", ERROR, PARSER);
										printTokenType(*cur);
										exit(synt);
								}
								// ulozeni odkazu na strukturu dat
								var->other = dta;		
								
								// vlozeni nazev pole do tabulky
								new = insertValue(&top, var->data.str);
								if(!new)
									return NULL;
								
								// nastaveni ze se jedna o pole
								new->type = DT_ARR;													
							}
							else {
								Log("vars: Syntax error - expected left brace", ERROR, PARSER);
								printTokenType(*cur);
								exit(synt);
							}
							
							break;
						}
					}	
				}
				
				
				(*cur) = getToc();
				if((*cur)->type != T_SCOL){
					Log("vars: Syntax error - expected semicolon after var deklaration", ERROR, PARSER);
					printTokenType(*cur);
					exit(synt);
				}
				
				
				// vytvoreni noveho AST v pripade, ze jeste nebyl vytvoren
				if(node == NULL){
					if(!(node = makeNewAST()))
						return NULL;
				}
				
				// vlozit prvek
				struct astNode* item = node;
				while(item->left != NULL){
					item = item->left;
				}
				item->left = makeNewAST();
				item->type = AST_NONE;
				item->right = var;
			}
			else if((*cur)->type == T_KW_BEGIN){
				// zacina telo 	
				break;
			}
			else if((*cur)->type == T_KW_FUNC){
				// zacina definice funkce	
				break;				
			}
			else {
				Log("vars: Syntax error - unsupported token", ERROR, PARSER);
				printTokenType(*cur);
				exit(synt);			
			}
		}
		
		if(!stackPush(global.symTable, top))
			return NULL;
				
		// vratit seznam promennych
		return node;
	}
 
	return NULL;
}

/**
 * Parsuje parametry v definici/deklaraci funkce
 * ---------------------------------------------
 */
struct astNode* parseParams(){
	Log("parseParams", WARNING, PARSER);
	
	struct toc* cur = getToc();
	struct astNode* params = makeNewAST();
	if(!params)
		return NULL;
		
		
	if(cur->type != T_LPAR){
		Log("params: Syntax error - expected Left parenthesis", ERROR, PARSER);
		printTokenType(cur);
		exit(synt);	
	}
	
	// nelezl levou zavorku -> nacitat dokud nenarazi na pravou zavorku
	cur = getToc();
	if(cur->type != T_RPAR)	{
		while(1){
			if(cur->type == T_ID){
				// zacina se cist parametr
				// <id>: <type>;				
				struct astNode* par = makeNewAST();
				
				// skopirovani jmena parametru
				if(!(copyString(cur->data.str, &(par->data.str))))
					return NULL;
				
				// nacist dvojtecku
				cur = getToc();
				if(cur->type != T_COL){
					Log("params: Syntax error - expected COLON after identificator", ERROR, PARSER);
					printTokenType(cur);
					exit(synt);				
				}
				
				// nacteni datoveho typu parametru
				cur = getToc();
				switch(cur->type){
					case T_KW_INT: {
						par->dataType = DT_INT;
						break;
					}
					case T_KW_REAL: {
						par->dataType = DT_REAL;
						break;
					}
					case T_KW_BOOLEAN: {
						par->dataType = DT_BOOL;
						break;
					}
					case T_KW_STRING: {
						par->dataType = DT_STR;
						break;
					}
					default:
						Log("params: Syntax error - unsupported type of parameter", ERROR, PARSER);
						printTokenType(cur);
						exit(synt);
				}
				
				// ulozit do seznamu a pokracovat
				struct astNode* item = params;
				while(item->left != NULL)
					item = item->left;
				item->left = par;				
				
				// ukonceni parametru
				cur = getToc();				
				if(cur->type == T_SCOL){
					// oddelovac parametru
					continue;				
				}
				else if(cur->type == T_RPAR){
					// ukonceni parametru
					break;					
				}			
			}
			else {
				Log("params: Syntax error - expected identificator of parameter", ERROR, PARSER);
				printTokenType(cur);
				exit(synt);	
			}
		
			cur = getToc();
		}
	}
	// pouze v pripade, ze byla zadana prava zavorka vratit seznam
	if(cur->type == T_RPAR){
		return params;	
	}
	return NULL;	
}

/**
 * Parsuje dopredne deklarace / definice funkci
 * --------------------------------------------
 */
struct astNode* parseFunction(){
	Log("parseFunction", WARNING, PARSER);
	// definice funkce / dopredna definice --- FUNCTION je uz nactene
	// FUNCTION <id> <params>: <type>; <var_def> <body>     
	// FUNCTION <id> <params>: <type>; FORWARD;
	
	struct astNode* node = makeNewAST();
	node->type = AST_FUNC;
	
	struct toc* cur = getToc();
	if(cur->type != T_ID){
		Log("function: Syntax error - expected ID after FUNCTION", ERROR, PARSER);
		printTokenType(cur);
		exit(synt);
	}
	
	// skopirovani jmena
	if(!(copyString(cur->data.str, &(node->data.str))))
		return NULL;
	
	// sparsovat parametry
	node->right = parseParams();
	
	cur = getToc();
	if(cur->type != T_COL){
		Log("function: Syntax error - expected colon after function params", ERROR, PARSER);
		printTokenType(cur);
		exit(synt);	
	}

	// dostal dvojtecku -> typ
	cur = getToc();
	switch(cur->type){
		case T_KW_INT: {
			// navratovy typ
			node->dataType = DT_INT;
			break;
		}
		case T_KW_REAL: {
			node->dataType = DT_REAL;			
			break;
		}
		case T_KW_BOOLEAN: {
			node->dataType = DT_BOOL;
			break;
		}
		case T_KW_STRING: {
			node->dataType = DT_STR;		
			break;
		}
		default: {
			Log("function: Syntax error - unsupported returning type of function", ERROR, PARSER);
			printTokenType(cur);
			exit(synt);
		}
	}	
	// typ OK

	
	// prohledani tabulky funkci
	struct symbolTableNode* dekl = (struct symbolTableNode*)search(&(global.funcTable), node->data.str);
	
	cur = getToc();
	if(cur->type == T_KW_FORW){
		// dopredna deklarace - pokud ji najde ve funkcich je neco spatne
	 	if(!dekl)
		 	dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), node->data.str);
		else {
			// nalezl deklarovanou/definovanou funkci v tabulce - chyba!
			Log("function: Syntax error - redefinition of forward declaration", ERROR, PARSER);
			exit(synt);
		}
			
	 	// nastaveni navratovy typ
	 	dekl->type = node->dataType;
	 	// v node->right jsou parametry
	 	dekl->other = node;
	 	return node;
	}
	
	// TODO vyresit definici tela a promennych funkce!
	// Pokud funkce bude nalezena - dopredna deklarace
	// 			pak nahrat telo a promenne do uzlu
	// Pokud funkce nebude nalezena - jedna se rovnou o definici funkce
	// 			vytvorit novy uzel v tabulce symbolu
	// 			pak nahrat telo a promenne do uzlu
	if(!(dekl)) {
		// nebyla nalezena - vlozit novy uzel
		if(!(dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), node->data.str)))
			return NULL;
	}
		
	if(dekl->other != NULL) {	
		if(((struct symbolTableNode*)dekl->other)->left != NULL){
			// telo uz bylo jednou definovane!!
			Log("function: Syntax error - redefinition of function", ERROR, PARSER);
			exit(sem_prog);
		}
	}
	
	
	// ocekavat var-def --> other
	node->other = parseVars(&cur);
	// po definici promennych MUSI nasledovat telo, tedy begin
	if(cur->type != T_KW_BEGIN)
		return NULL;
		
	// ocekavat telo
	node->left = parseBody(&cur);	
			
	return node;
}

/**
 * Parsuje parametry volani funkce a vklada je do stromu, smerem doleva
 * --------------------
 */
struct astNode* parseCallParams(){
	Log("parseCallParams", WARNING, PARSER);
	// prochazi podel parametru a vytvari doleva jdouci strom z parametru
	struct astNode* node = makeNewAST();
	if(!node)
		return NULL;
	
	struct toc* cur = getToc();
	while(cur->type != T_RPAR){
		struct astNode* nd;
		
		switch(cur->type){
			case T_ID: {
				// predana promenna
								
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_ID;
				if(!(copyString(cur->data.str, &(nd->data.str))))
					return NULL;

				break;
			}
			case T_INT: {
				// predany parametr je typu INT
				
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_INT;
				nd->dataType = DT_INT;
				nd->data.integer = cur->data.integer;
							
				break;
			}
			case T_REAL: {
				// predany parametr typu real
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_REAL;
				nd->data.real = cur->data.real;
				
				break;
			}
			case T_BOOL: {
				// predany parametr typu bool
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_BOOL;
				nd->data.boolean = cur->data.boolean;
				
				break;
			}
			case T_STR: {
				// predany parametry typu string
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_STR;
				if(!(copyString(cur->data.str, &(nd->data.str))))
					return NULL;
		
				break;
			}
			default:
				Log("callPars: Syntax error - invalid parameter type", ERROR, PARSER);
				printTokenType(cur);
				exit(synt);
		}
		
		// nacteni oddelovace
		cur = getToc();
		if(cur->type == T_COM){
			// nacetl carku - v poradku pokud nenasleduje T_RPAR
			cur = getToc();
			if(cur->type == T_RPAR){
				Log("callPars: Syntax error - comma before right parenthesis", ERROR, PARSER);
				printTokenType(cur);
				exit(synt);
			}
		}
		
		// vytvoreni polozky a vlozeni polozky do node
		struct astNode* item = node;
		while(item->left != NULL){
			item = item->left;
		}
		// vlozeni do praveho uzlu parametr
		item->left = makeNewAST();
		item->left = nd;
	}
	return node;
}

/**
 * Parsuje volani funkce, kde uz dostane predany T_ID od exprParseru
 * --------------------
 * id: Token ID se jmenem funkce
 */
struct astNode* parseFuncCall(struct toc* id){
	Log("parseFuncCall", WARNING, PARSER);
	// v cyklu nasel T_ID a nasledne T_LPAR --> pravdepodobne volani funkce
	
	struct astNode* node = makeNewAST();
	if(!node)
		return NULL;
		
	// skopirovani jmena promenne
	if(!(copyString(id->data.str, &(node->data.str))))
		return NULL;
	
	if(!(node->right = parseCallParams()))
		return NULL;
	node->left = NULL;
	node->other = NULL;
	node->type = AST_FUNC;
	
	return node;
}

/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * ------------------------------------------------------------------------------------
 */
struct astNode* parseCommand(struct toc** cur){
	Log("parseCommand", WARNING, PARSER);
	(*cur) = getToc();
	
	printTokenType(*cur);
	switch((*cur)->type){
		case T_KW_IF: {			
			struct astNode* ifstat = ifStatement(cur);
			if(!ifstat)
				return NULL;

			return ifstat;			
		}
		case T_KW_WHILE: {
			struct astNode* whl = whileStatement();
			if(!whl)
				return NULL;
				
			return whl;			
		}
		case T_RPT: {
			struct astNode* rpt = repeatStatement(cur);
			if(!rpt)
				return NULL;
				
			return rpt;
		}
		case T_KW_WRT: {
			/* content */
			break;
		}
		case T_KW_READLN: {
			/* content */
			break;
		}
		case T_KW_FIND: {
			/* content */
			break;
		}
		case T_KW_SORT: {
			/* content */
			break;
		}
		case T_KW_LENGTH: {
			/* content */
			break;
		}
		case T_KW_COPY: {
			/* content */
			break;
		}
		case T_ID: {
			struct toc* next = getToc();
			if(next->type == T_LPAR){
				// pravdepodobne volani funkce
				return parseFuncCall(*cur);
			}
			else if(next->type == T_ASGN){
				// pravdepodobne prirazeni
				printTokenType(*cur);
				printTokenType(next);
				
				// levy uzel je T_ID
				// pravy uzel je expression
				struct astNode* left = makeNewAST();
				if(!left)
					return NULL;
					
				left->type = AST_ID;
				if(!copyString((*cur)->data.str, &(left->data.str)))
					return NULL;
				
				// prava strana je vyraz
				struct astNode* right = parseExpression(cur);
				if(!right) 
					return NULL;
				
				// vytvorit uzel prirazeni a vratit jej
				struct astNode* asgn = makeNewAST();
				asgn->type = AST_ASGN;
				asgn->left = left;
				asgn->right = right;
				asgn->other = NULL;
				return asgn;
			}
			else {
				Log("cmd: Syntax error - expected funcCall or assign", ERROR, PARSER);
				printTokenType(*cur);
				exit(synt);
			}		
			break;
		}
		case T_KW_END: {
			return NULL;
		}
	}	
	
	return NULL;
}

/**
 * Parsuje IF statement	
 * --------------------
 * cur: pokud bude cur nastavene na strednik nebo end
 */
struct astNode* ifStatement(struct toc** cur){
	// nasleduje podminka
	struct astNode* condition = parseExpression(cur);
	if(!condition)
		return NULL;
		
	if((*cur)->type != T_KW_THEN){
		Log("if: Syntax error - expected THEN keyword at the end of condition", ERROR, PARSER);
		printTokenType(*cur);
		exit(synt);
	}
		
	// vytvoreni nove polozky
	struct astNode* ifnode = makeNewAST();
	if(!ifnode)
		return NULL;
	// nastaveni znamych informaci
	ifnode->type = AST_IF;
	ifnode->other = condition;
	
	// telo true
	ifnode->left = parseBody(cur);
	if(!ifnode->left)
		return NULL;
			
	// ulozeni dalsiho tokenu
	(*cur) = getToc();
	
	// pokud else pak pokracovat telem
	if((*cur)->type == T_KW_ELSE){
		ifnode->right = parseBody(cur);
		if(!ifnode->right)
			return NULL;
	}
		
	return ifnode;
}

/**
 * Parsuje WHILE statement
 * --------------------
 * Name:  Info
 */
struct astNode* whileStatement(){
	struct toc* cur;
	struct astNode* condition = parseExpression(&cur);
	if(!condition)
		return NULL;
	
	if(cur->type != T_KW_DO){
		Log("while: Syntax error - expected DO keyword at the end of condition", ERROR, PARSER);
		printTokenType(cur);
		exit(synt);
	}

	struct astNode* node = makeNewAST();				
	if(!node)
		return NULL;
	// ulozeni podminky
	node->other = condition;
	node->type = AST_WHILE;
	
	// kompletace tela
	node->left = parseBody(&cur);
	if(!node->left)
		return NULL;
	node->right = NULL;
	
	// navraceni stromu 
	return node;
}

/**
 * Parsuje REPEAT cyklus
 * --------------------
 * cur: Vraci tudy posledni nacteny token
 */
struct astNode* repeatStatement(struct toc** cur){
	struct astNode* rpt = makeNewAST();
	if(!rpt)
		return NULL;
	
	// telo cyklu
	rpt->left = parseBody(cur);
	if(!rpt->left)
		return NULL;
		
	// rpt->left == AST_CMD
	// rpt->left->right == prikaz tela
	if(rpt->left->right == NULL){
		Log("repeat: Syntax error - repeat cycle must have at least one command in the body", ERROR, PARSER);
		exit(synt);
	}
	
	(*cur) = getToc();
	if((*cur)->type != T_UNTIL){
		Log("repeat: Syntax error - expected UNTIL keyword", ERROR, PARSER);
		printTokenType(*cur);
		exit(synt);
	}
	
	// nacetl UNTIL
	rpt->other = parseExpression(cur);
	if(!rpt->other)
		return NULL;
	
	return rpt;
}

/**
 * Parsuje vyraz, aritmeticko-logicky
 * --------------------
 * endToken:
 *		urcuje, kterym tokenem ma nacitani skoncit
 */
struct astNode* parseExpression(struct toc** cur){
	Log("parseExpression", WARNING, PARSER);
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// zasobnik pro chystani AST
	struct stack* aststack = makeNewStack();				
	
	// zacatek podminky
	(*cur) = getToc();
		
	// <id>(
	// <id> <operator> (
	if((*cur)->type == T_ID){
		// odklad na ID token
		struct toc* id = (*cur);
		
		(*cur) = getToc();
		if((*cur)->type == T_LPAR){
			// volani funkce -> za ID pokracovala leva zavorka	
			
			// <id>(.....	
			return parseFuncCall(id);
		}
		else {
			// v pripade, ze po ID nebyla zavorka
			// pushnout na stack, jako promennou
			struct astNode* node = makeNewAST();
			node->type = AST_ID;
			// kopie jmena
			if(!copyString(id->data.str, &(node->data.str)))
				return NULL;
				
			node->left = NULL;
			node->right = NULL;
			
			if(!stackPush(aststack, id))
				return NULL;	
		}	
	}
	
	// 1. operand -> outqueue
	// 2. leva zavorka -> na vrchol zasobniku
	// 3. operator:
	// 		pokud je zasobnik prazdny
	// 		pokud je na vrcholu leva zavorka
	// 		pokud je na vrcholu operator s nizsi prioritou
	// pokud je na vrcholu operator s vyssi prioritou -> 
	// 		presunout na vystup a opakovat 3. krok
	// 4. prava zavorka -> odebirat ze zasobniku a davat na vystup
	// 		dokud nenarazi na levou zavorku
	// 5. v pripade jineho tokenu -> vyprazdnovat zasobnik na vystup
	// 		tokeny ;, THEN, DO by mely vesmes ukoncovat vyrazy	
	while((*cur) != NULL){
		switch((*cur)->type){
			// leva zavorka
			case T_LPAR: {
				Log("T_LPAR comes", WARNING, PARSER);
			
				if(!stackPush(stack, (*cur)))
					return NULL;
				break;
			}
			// prava zavorka
			case T_RPAR: {
				Log("T_RPAR comes", WARNING, PARSER);
			
				struct toc* t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					Log("expression: Syntax error - no Left parenthesis before Right parenthesis", ERROR, PARSER);
					exit(synt);
				}
				
				while(!stackEmpty(stack) && (t->type != T_LPAR)){
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					if(!makeAstFromToken(t, &aststack))
						return NULL;	
					
					t = (struct toc*)stackPop(stack);				
					printTokenType(t);
				}				
				break;
			}
			// operandy
			case T_ID:
			case T_INT:
			case T_REAL:
			case T_STR:
			case T_BOOL: 
			case T_KW_TRUE:
			case T_KW_FALSE:{
				Log("Literal comes", WARNING, PARSER);
				struct astNode* node = makeNewAST();
				
				if((*cur)->type == T_ID){
					node->type = AST_ID;
					// kopie jmena
					if(!copyString((*cur)->data.str, &(node->data.str)))
						return NULL;
				}
				else if((*cur)->type == T_INT){
					node->type = AST_INT;
					node->data.integer = (*cur)->data.integer;				
				}
				else if((*cur)->type == T_REAL){
					node->type = AST_REAL;
					node->data.real = (*cur)->data.real;
				}
				else if((*cur)->type == T_BOOL){
					node->type = AST_BOOL;
					node->data.boolean = (*cur)->data.boolean;
				}
				else if((*cur)->type == T_STR){
					node->type = AST_STR;
					if(!copyString((*cur)->data.str, &(node->data.str)))
						return NULL;
				}
				else if((*cur)->type == T_KW_TRUE){
					node->type = AST_BOOL;
					node->data.boolean = True;
				}
				else if((*cur)->type == T_KW_FALSE){
					node->type = AST_BOOL;
					node->data.boolean = False;
				}

				// pushnout vysledek
				if(!stackPush(aststack, node))
					return NULL;			
				break;
			}
			case T_EQV:
			case T_NEQV:
			case T_GRT:
			case T_LSS:
			case T_GEQV:
			case T_LEQV:
			case T_ADD:
			case T_SUB:
			case T_MUL:
			case T_DIV:
			case T_AND:
			case T_OR:
			case T_XOR:
			case T_NOT: {
				Log("Operator comes", WARNING, PARSER);
				printTokenType(*cur);
				// 3. operator:
				// 		pokud je zasobnik prazdny
				// 		pokud je na vrcholu leva zavorka
				// 		pokud je na vrcholu operator s nizsi prioritou
				// pokud je na vrcholu operator s vyssi prioritou -> 
				// 		presunout na vystup a opakovat 3. krok
			
				struct toc* top;
				if(stackEmpty(stack)){
					// v pripade, ze je to prvni zaznam do stacku, tak ho tam proste vrazit
					if(!stackPush(stack, (*cur)))
						return NULL;
				}
				else {	
					top = (struct toc*)stackTop(stack);
				
					// neni to prvni zapis, nutno zkontrolovat priority
					while(!stackEmpty(stack) && (getPriority(top) >= getPriority(*cur))){
						// odebrat vrchol ze zasobniku
						top = (struct toc*)stackPop(stack);
						
						// vytvori ASTnode z tokenu
						if(!makeAstFromToken(top, &aststack))
							return NULL;	
					
						// na promennou za vrchol ulozi top prvek
						top = (struct toc*)stackTop(stack);
					}		
					
					// ulozit aktualni prvek na vrchol zasobniku
					if(!stackPush(stack, (*cur)))
						return NULL;
				}	
				break;
			}
			default:
				// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
				while(!stackEmpty(stack)){
					struct toc* now = (struct toc*)stackPop(stack);
					if(!makeAstFromToken(now, &aststack))
						return NULL;
				}
				
			
				// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
				if(aststack->Length > 1){
					Log("expression: Shunting yard error - stack length is grather then 1", ERROR, PARSER);
					exit(intern);
				}
				else if(aststack->Length == 0){
					Log("expression: Shunting yard error - stack is empty", ERROR, PARSER);
					exit(intern);
				}
			
				// vrati vrchol 
				return stackPop(aststack);				
		}
		
		// get next token
		(*cur) = getToc();
	}
	return NULL;
}
