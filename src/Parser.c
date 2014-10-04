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
		if(token->type == tokennames[i].type)
			fprintf(stderr, "Token: %s\n", tokennames[i].str);
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

struct astNode* parseParams(){
	struct toc* cur = getToc();
	struct astNode* params = makeNewAST();
	if(!params)
		return NULL;
		
		
	if(cur->type != T_LPAR){
		Log("Syntax error - expected Left parenthesis", ERROR, PARSER);
		global.errno = synt;
		return NULL;	
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
					Log("Syntax error - expected COLON after identificator", ERROR, PARSER);
					global.errno = synt;
					return NULL;				
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
						Log("Syntax error - unsupported type of parameter", ERROR, PARSER);
						global.errno = synt;
						return NULL;
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
				Log("Syntax error - expected identificator of parameter", ERROR, PARSER);
				global.errno = synt;
				return NULL;	
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
 * Parsuje var sekci funkce/programu
 * --------------------
 */
struct astNode* parseVars(struct toc** readToc){
	struct toc* cur = getToc();
	if(cur->type == T_KW_VAR){
		// zacinat parsovat promenne
		
		struct astNode* node;
		while(1){
			// definice promennych
			struct astNode* var = makeNewAST();
			cur = getToc();
			if(cur->type == T_ID){
				// dostal jsem ID -> ocekavam dvoutecku
			
				if(!(copyString(cur->data.str, &(var->data.str))))
					return NULL;
			
				// odkladiste pro nove promenne do tabulky symbolu				
				struct symbolTableNode* new = NULL;
				
				if((cur = getToc())->type == T_COL){
					// dostal dvojtecku --> ocekavat typ
					
					// vytazeni TOP vrstvy
					struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
				
					cur = getToc();
					switch(cur->type){
						case T_KW_INT: {
							var->type = AST_INT;
							
							// vytvorit novy zaznam v tabulce
							new = insertValue(&top, cur->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_INT;													
							break;
						}
						case T_KW_REAL: {
							var->type = AST_REAL;
						
							new = insertValue(&top, cur->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_REAL;
							break;
						}
						case T_KW_BOOLEAN: {
							var->type = AST_BOOL;
						
							new = insertValue(&top, cur->data.str);
							if(!new)
								return NULL;
								
							new->type = DT_BOOL;
							break;
						}
						case T_KW_STRING: {
							var->type = AST_STR;
							
							new = insertValue(&top, cur->data.str);
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
							if(!(copyString(cur->data.str, &(dta->id))))
								return NULL;
							
							cur = getToc();
							if(cur->type == T_LBRC) {
								// leva [ -> ocekavat integer								
								cur = getToc();
								if(cur->type != T_INT){
									Log("Syntax error - expected integer as lower range of array index", ERROR, PARSER);
									global.errno = synt;
									return NULL;
								}
								dta->low = cur->data.integer;
								
								// interval - dve tecky mezi integery
								cur = getToc();
								if(cur->type != T_DDOT){
									Log("Syntax error - expected two dots", ERROR, PARSER);
									global.errno = synt;
									return NULL;
								}
								
								// ocekavat druhy integer
								cur = getToc();
								if(cur->type != T_INT){
									Log("Syntax error - expected integer as higher range of array index", ERROR, PARSER);
									global.errno = synt;
									return NULL;
								}
								dta->high = cur->data.integer;
								
								// ocekavam konec intervalu
								cur = getToc();
								if(cur->type != T_RBRC){
									Log("Syntax error - expected right brace", ERROR, PARSER);
									global.errno = synt;
									return NULL;
								}
								
								// ocekavat OF
								cur = getToc();
								if(cur->type != T_OF){
									Log("Syntax error - expected OF for defining type of array", ERROR, PARSER);
									global.errno = synt;
									return NULL;
								}
								
								// nacteni typu promenne
								cur = getToc();
								switch(cur->type){
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
										Log("Syntax error - unsupported type of array", ERROR, PARSER);
										global.errno = synt;
										return NULL;
								}
								// ulozeni odkazu na strukturu dat
								var->other = dta;		
								
								// vrchol tabulky
								struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
								if(!top)
									return NULL;
								
								// vlozeni nazev pole do tabulky
								new = insertValue(&top, var->data.str);
								if(!new)
									return NULL;
								
								// nastaveni ze se jedna o pole
								new->type = DT_ARR;													
							}
							else {
								Log("Syntax error - expected left brace", ERROR, PARSER);
								global.errno = synt;
								return NULL;
							}
							
							break;
						}
					}
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
			else if(cur->type == T_KW_BEGIN){
				// zacina telo 	
				(*readToc) = cur;
				break;
			}
			else if(cur->type == T_KW_FUNC){
				// zacina definice funkce	
				(*readToc) = cur;
				break;				
			}
			else {
				Log("Syntax error - unsupported token", ERROR, PARSER);
				global.errno = synt;
				return NULL;			
			}
		}
		
		// vratit seznam promennych
		return node;
	}
 
	return NULL;
}
/**
 * Parsuje dopredne deklarace / definice funkci
 * --------------------------------------------
 */
struct astNode* parseFunction(){
	// definice funkce / dopredna definice --- FUNCTION je uz nactene
	// FUNCTION <id> <params>: <type>; <var_def> <body>     
	// FUNCTION <id> <params>: <type>; FORWARD;
	
	struct astNode* node = makeNewAST();
	node->type = AST_FUNC;
	
	struct toc* cur = getToc();
	if(cur->type != T_ID){
		Log("Syntax error - expected ID after FUNCTION", ERROR, PARSER);
		global.errno = synt;
		return NULL;
	}
	
	// skopirovani jmena
	if(!(copyString(cur->data.str, &(node->data.str))))
		return NULL;
	
	// sparsovat parametry
	node->right = parseParams();
	if(global.errno != ok)
		return NULL;
	
	cur = getToc();
	if(cur->type != T_COL){
		Log("Syntax error - expected colon after function params", ERROR, PARSER);
		global.errno = synt;
		return NULL;	
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
			Log("Syntax error - unsupported returning type of function", ERROR, PARSER);
			global.errno = synt;
			return NULL;
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
			Log("Syntax error - redefinition of forward declaration", ERROR, PARSER);
			global.errno = synt;
			return NULL;
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
	if(!(dekl)) {// nebyla nalezena - vlozit novy uzel
		if(!(dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), node->data.str)))
			return NULL;
	}
		
	if(dekl->other != NULL) {	
		if(((struct symbolTableNode*)dekl->other)->left != NULL){
			// telo uz bylo jednou definovane!!
			Log("Syntax error - redefinition of function", ERROR, PARSER);
			global.errno = sem_prog;
			return NULL;
		}
	}
	
	
	// ocekavat var-def --> other
	node->other = parseVars(&cur);
	// po definici promennych MUSI nasledovat telo, tedy begin
	if(global.errno != ok || cur->type != T_KW_BEGIN)
		return NULL;
		
	// ocekavat telo
	node->left = parseBody(True);	
	if(global.errno != ok)
		return NULL;
		
	return node;
}
/**
 * Kontroluje zacatek programu, existenci tokenu program, jmeno programu a nasledne telo
 * -------------------------------------------------------------------------------------
 */
struct astNode* parseProgram(){
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
		global.errno = synt;
		return NULL;
	}
	
	// jmeno programu
	struct astNode* program = makeNewAST();
	// kopirovani jmena programu
	if(!(copyString(cur->data.str, &(program->data.str))))
		return NULL;			
	
	cur = getToc();
	if(cur->type != T_SCOL){
		Log("Syntax error - expected SEMICOLON", ERROR, PARSER);
		global.errno = synt;
		return NULL;
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
	// lokalni promenne
	program->other = vardef;
		
	// prochazet definice funkci a vkladat je s telem do tabulky symbolu
	while(cur->type == T_KW_FUNC){
		struct astNode* func = parseFunction();				
		if(!func)
			return NULL;
		
		cur = getToc();				
	}
			
	// nasleduje telo programu
	struct astNode* body = parseBody(False);		
	if(!body)
		return NULL;
	program->left = body;
	
	cur = getToc();
	if(cur->type != T_KW_ENDDOT){
		Log("Syntax error - expected program end", ERROR, PARSER);
		global.errno = synt;
		return NULL;
	}
			
	return program;
}

/**
 * Parsuje parametry volani funkce a vklada je do stromu, smerem doleva
 * --------------------
 */
struct astNode* parseCallParams(){
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
				Log("Syntax error - invalid parameter type", ERROR, PARSER);
				global.errno = synt;
				return NULL;
		}
		
		// nacteni oddelovace
		cur = getToc();
		if(cur->type == T_COM){
			// nacetl carku - v poradku pokud nenasleduje T_RPAR
			cur = getToc();
			if(cur->type == T_RPAR){
				Log("Syntax error - coma before right parenthesis", ERROR, PARSER);
				global.errno = synt;			
				return NULL;
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
 * Parsuje telo, pouze zkontroluje zda-li zacina na begin a konci na end.
 * --------------------
 */
struct astNode* parseBody(int beginread){
	struct toc* cur;
	
	if(!beginread)	
		cur = getToc();
	if(cur->type != T_KW_BEGIN){
		Log("Syntax error - expected BEGIN token", ERROR, PARSER);
		global.errno = synt;
		return NULL;
	}
		
	// zacatek tela -> spustit prikazy
	// ukladat je doleva za sebe do typu AST_CMD
	bool endToken;
	struct astNode* body = makeNewAST();
	struct astNode* cmd;
	
	while((cmd = parseCommand(&endToken)) != NULL){
		struct astNode* item = body;
		while(item != NULL)
			item = item->left;
		
		item = makeNewAST();
		item->type = AST_CMD;
		item->left = NULL;
		item->right = cmd;		
	}
	
	if(!body)
		return NULL;
	
	
			
	// nacteni konce END
	cur = getToc();
	if(cur->type == T_KW_END)
		return body;
	else {
		Log("Syntax error - expected END token at the end of body", ERROR, PARSER);
		global.errno = synt;
		return NULL;
	}
	
	return NULL;
}
/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * --------------------
 * TODO Volani funkci apod.
 */
struct astNode* parseCommand(bool* end){
	struct toc* cur = getToc();
	
	switch(cur->type){
		case T_KW_IF: {
			struct astNode* ifnode;
		
			// nasleduje podminka
			struct astNode* condition = parseExpression(T_KW_THEN);
			if(!condition)
				return NULL;
				
			if(!(ifnode = makeNewAST()))
				return NULL;
			ifnode->type = AST_IF;
			ifnode->other = condition;
			
			// telo true
			if(!(ifnode->left = parseBody(False)))
				return NULL;
				
			// telo else -- 
			cur = getToc();
			if(cur->type == T_KW_ELSE){
				if(!(ifnode->right = parseBody(False)))
					return NULL;
			}
			else if(cur->type == T_KW_END){
				// kratky if
				ifnode->right = NULL;
				return ifnode;					
			}		
			else {
				Log("Syntax error - expected ELSE or END", ERROR, PARSER);
				global.errno = synt;
				return NULL;		
			}
			break;
		}
		case T_KW_WHILE: {
			struct astNode* node;
			
			struct astNode* condition = parseExpression(T_KW_DO);
			if(!condition)
				return NULL;
				
			if(!(node = makeNewAST()))
				return NULL;
			// ulozeni podminky
			node->other = condition;
			node->type = AST_WHILE;
			
			// kompletace tela
			if(!(node->left = parseBody(False)))
				return NULL;
			node->right = NULL;
			// navraceni stromu 
			return node;
		}
		case T_RPT: {
			// nejdriv je telo a pak podminka!!!!
			struct astNode* node = makeNewAST();
			if(!node)
				return NULL;
				
			// zjisteni tela - neni ohraniceno v BEGIN END!!!!!
			bool isEnd;
			struct astNode* cmd = parseCommand(&isEnd);
			while(cmd != NULL && !isEnd){
				
				
				
			
			}
				
			// ze zadani: Telo musi obsahovat alespon jeden prikaz
			if(node->left->type != AST_CMD){
				Log("Syntax error - repeat cycle has no commands", ERROR, PARSER);
				global.errno = intern;
				return NULL;
			}
			
			node->type = AST_REPEAT;
			
			cur = getToc();
			if(cur->type == T_UNTIL){
				// korektni pokracovani
				
				// precteni podminky
				if(!(node->other = parseExpression(T_SCOL)))
					return NULL;
				*end = True;
				
				node->right = NULL;
				return node;
			}
			else {
				Log("Syntax error - expected UNTIL", ERROR, PARSER);
				global.errno = synt;
				return NULL;			
			}
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
				
				return parseFuncCall(cur);
			}
			else if(next->type == T_ASGN){
				// pravdepodobne prirazeni
				
				// levy uzel je T_ID
				// pravy uzel je expression
				struct astNode* left = makeNewAST();
				if(!left)
					return NULL;
					
				left->type = AST_ID;
				if(!copyString(cur->data.str, &(left->data.str)))
					return NULL;
				
				// prava strana je vyraz
				struct astNode* right = parseExpression(T_SCOL);
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
				Log("Syntax error - expected funcCall or assign", ERROR, PARSER);
				global.errno = synt;
				return NULL;
			}		
			break;
		}
	}	
	
	return NULL;
}

/**
 * Parsuje vyraz, aritmeticko-logicky
 * --------------------
 * endToken:
 *		urcuje, kterym tokenem ma nacitani skoncit
 */
struct astNode* parseExpression(int endToken){
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// zasobnik pro chystani AST
	struct stack* aststack = makeNewStack();				
	// zacatek podminky
	struct toc* cur = getToc();
	
	
	// <id>(
	// <id> <operator> (
	
	if(cur->type == T_ID){
		struct toc* id = cur;
		
		cur = getToc();
		if(cur->type == T_LPAR){
			// volani funkce -> za ID pokracovala leva zavorka		
			return parseFuncCall(cur);
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
			
			if(!stackPush(aststack, node))
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
	while(cur != NULL){
		switch(cur->type){
			// leva zavorka
			case T_LPAR: {
				Log("T_LPAR comes", WARNING, PARSER);
			
				if(!stackPush(stack, cur))
					return NULL;
				break;
			}
			// prava zavorka
			case T_RPAR: {
				Log("T_RPAR comes", WARNING, PARSER);
			
				struct toc* t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					Log("Syntax error - no Left parenthesis before Right parenthesis", ERROR, PARSER);
					global.errno = synt;
					return NULL;
				}
				
				// TODO neco je tady spatne! opravit
				while(!stackEmpty(stack) && (t->type != T_LPAR)){
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					if(!makeAstFromToken(t, &aststack))
						return NULL;	
					
					t = (struct toc*)stackPop(stack);				
					printTokenType(t);
				}
				if(t->type == T_LPAR){
					Log("LPAR found", ERROR, PARSER);
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
				
				if(cur->type == T_ID){
					node->type = AST_ID;
					// kopie jmena
					if(!copyString(cur->data.str, &(node->data.str)))
						return NULL;
				}
				else if(cur->type == T_INT){
					node->type = AST_INT;
					node->data.integer = cur->data.integer;				
				}
				else if(cur->type == T_REAL){
					node->type = AST_REAL;
					node->data.real = cur->data.real;
				}
				else if(cur->type == T_BOOL){
					node->type = AST_BOOL;
					node->data.boolean = cur->data.boolean;
				}
				else if(cur->type == T_STR){
					node->type = AST_STR;
					if(!copyString(cur->data.str, &(node->data.str)))
						return NULL;
				}
				else if(cur->type == T_KW_TRUE){
					node->type = AST_BOOL;
					node->data.boolean = True;
				}
				else if(cur->type == T_KW_FALSE){
					node->type = AST_BOOL;
					node->data.boolean = False;
				}
										
				node->left = NULL;
				node->right = NULL;
			
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
				printTokenType(cur);
				// 3. operator:
				// 		pokud je zasobnik prazdny
				// 		pokud je na vrcholu leva zavorka
				// 		pokud je na vrcholu operator s nizsi prioritou
				// pokud je na vrcholu operator s vyssi prioritou -> 
				// 		presunout na vystup a opakovat 3. krok
			
				struct toc* top;
				if(stackEmpty(stack)){
					if(!stackPush(stack, cur))
						return NULL;
				}
				else {
					while(!stackEmpty(stack) && (getPriority(top) >= getPriority(cur))){
						top = (struct toc*)stackPop(stack);
						
						if(!makeAstFromToken(top, &aststack))
							return NULL;	
					
						top = (struct toc*)stackTop(stack);
					}		
					if(!stackPush(stack, cur))
						return NULL;
				}	
				break;
			}
			default:
				if(cur->type == T_KW_END || cur->type == endToken){
					if(cur->type == T_KW_END)
						Log("Shunting yard was stoped by END token", WARNING, PARSER);
										
					// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
					while(!stackEmpty(stack)){
						struct toc* now = (struct toc*)stackPop(stack);
						if(!makeAstFromToken(now, &aststack))
							return NULL;
					}
				
					// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
					if(aststack->Length > 1){
						Log("Shunting yard error - stack length is grather then 1", ERROR, PARSER);
						global.errno = intern;
						return NULL;
					}
					else if(aststack->Length == 0){
						Log("Shunting yard error - stack is empty", ERROR, PARSER);
						global.errno = intern;
						return NULL;
					}
				
					// vrati vrchol 
					return stackPop(aststack);
				}
			
				Log("Syntax error - invalid token type", ERROR, PARSER);	
				global.errno = synt;
				return NULL;	
		}
		
		// get next token
		cur = getToc();
	}
	return NULL;
}
