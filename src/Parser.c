#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Stack.h"
#include "Log.h"
#include "Ast.h"
#include "Scanner.h"
#include "Parser.h"
#include "SymbolTable.h"

#define D(t) { Log(t, DEBUG, PARSER); }
#define E(t) { Log(t, ERROR, PARSER); }
#define W(t) { Log(t, WARNING, PARSER); }

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
	for(int i=0; tokennames[i].str; i++){
		if(token->type == tokennames[i].type){
			fprintf(stderr, "Token: %s\n", tokennames[i].str);
			
			if(token->type == T_ID){
				fprintf(stderr, "\t%s\n", token->data.str->Value);
			}
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

void datatypes(int left, int right){
	char* l = malloc(sizeof(char));
	char* r = malloc(sizeof(char));
	
	switch(left){
		case DT_INT:
			sprintf(l, "DT_INT");
			break;
		case DT_REAL:
			sprintf(l, "DT_REAL");
			break;
		case DT_BOOL:
			sprintf(l, "DT_BOOL");
			break;
		case DT_STR:
			sprintf(l, "DT_STR");
			break;
		case DT_NONE:
			sprintf(l, "DT_NONE");
			break;
		case DT_ARR:
			sprintf(l, "DT_ARR");
			break;
		default:
			sprintf(l, "??????");
			break;
	}
	
	switch(right){
		case DT_INT:
			sprintf(r, "DT_INT");
			break;
		case DT_REAL:
			sprintf(r, "DT_REAL");
			break;
		case DT_BOOL:
			sprintf(r, "DT_BOOL");
			break;
		case DT_STR:
			sprintf(r, "DT_STR");
			break;
		case DT_NONE:
			sprintf(r, "DT_NONE");
			break;
		case DT_ARR:
			sprintf(r, "DT_ARR");
			break;
		default:
			sprintf(l, "??????");
			break;
	}
	
	fprintf(stderr, "%s< Comparison between %s and %s >%s\n", ((right==left)?COLOR_LGRN:COLOR_LRED), l, r, COLOR_NRM);	
	free(l);
	free(r);
}
int valid(struct astNode* left, struct astNode* right, int op){	
	struct symbolTableNode* symtable = (struct symbolTableNode*)stackTop(global.symTable);
	
	// dodatecne typy leve strany
	if(left->type == AST_ID){
		struct symbolTableNode* id = search(&symtable, left->data.str);
		if(id == NULL){
			E("valid: ID not found in symtable");
			exit(intern);
		}
		
		if(left->dataType != id->dataType){	
			W("valid: data types are not same");
			left->dataType = id->dataType;
		}
	}
	else if(left->type == AST_FUNC){
		struct symbolTableNode* func = search(&global.funcTable, left->data.str);
		if(func == NULL){
			E("valid: Function not found");
			exit(intern);
		}
		
		if(left->dataType != func->dataType){
			W("valid: data types are not same (func)");
			left->dataType = func->dataType;
		}
	}
	
	// Dodatecne typy prave strany
	if(right->type == AST_ID){
		struct symbolTableNode* id = search(&symtable, right->data.str);
		if(id == NULL){
			E("valid: ID not found in symbol table");
			exit(intern);
		}
		
		if(right->dataType != id->dataType){
			W("valid: data types are not same");
			exit(sem_komp);
		}
	}
	else if(right->type == AST_FUNC){
		struct symbolTableNode* func = search(&global.funcTable, right->data.str);
		if(func == NULL){
			E("valid: Function not found");
			exit(intern);
		}
		
		if(right->dataType != func->dataType){
			E("valid: data types are not same");
			right->dataType = func->dataType;
		}
	}
	
	
	switch(op){
		case AST_ADD:
			if(left->dataType == DT_STR && right->dataType == DT_STR) {
				W("valid: String cat");
				return DT_STR;
			}
		case AST_SUB:
		case AST_MUL:
		case AST_DIV:
			if((left->dataType == DT_INT || left->dataType == DT_REAL) && (right->dataType == DT_INT || right->dataType == DT_REAL)){
				// MATH				
				if(op == AST_DIV){
					// operace deleni
					return DT_REAL;
				}			
				else {
					// ostatni operace - porovnani podle typu
					if(left->dataType == DT_INT && right->dataType == DT_INT)
						return DT_INT;
					else 
						return DT_REAL;		
				}
			} 
			else {
				E("valid: Arithmetic operator needs INT or REAL data");
				exit(sem_komp);			
			}
			break;
		case AST_EQV:
		case AST_NEQV:
		case AST_LSS:
		case AST_GRT:
		case AST_LEQV:
		case AST_GEQV:
			// logicke operatory
			// hodnoty ocekava matematicke, logicke i string
			// hodnoty musi byt stejneho typu aby se mohly porovnavat	
			if(left->dataType != right->dataType){
				// pokud nemaji stejne typy
				E("valid: Semantic error - no same data types on both sides");
				datatypes(left->dataType, right->dataType);
				exit(sem_komp);
			}
			
			// stejne typy na obou stranach
			if(left->dataType == DT_INT || left->dataType == DT_REAL || left->dataType == DT_BOOL || left->dataType == DT_STR){
				// vsechny moznosti
				
				if(op == AST_EQV || op == AST_NEQV){
					// pro vsechny
					return DT_BOOL;
				}
				else {
					// ostatni operace - MATH operands < > <= >= 
					if(left->dataType == DT_BOOL || left->dataType == DT_STR){
						E("valid: Semantic error - invalid operands");
						datatypes(left->dataType, right->dataType);
						exit(sem_komp);
					}
					
					return DT_BOOL;
				}			
			}
			else {
				// v pripade nepodporovanych datovych typu
				E("valid: Semantic error - unsupported data types");
				datatypes(left->dataType, right->dataType);
				exit(sem_komp);
			}
			
			break;
		case AST_AND:
		case AST_OR:
		case AST_XOR:
			if(left->dataType == DT_BOOL && left->dataType == right->dataType){
				return DT_BOOL;
			}
			
			E("valid: Semantic error - logic operators needs logic operands");
			datatypes(left->dataType, right->dataType);
			exit(sem_komp);
			break;
		case AST_NOT:
			if(right != NULL){
				E("valid: Semantic error - NOT operator has only one operand");
				datatypes(left->dataType, right->dataType);
				exit(sem_komp);
			}
			break;
		case AST_ASGN:
			if(right == left)
				return right->dataType;
			else{
				E("valid: Semantic error - not same data types");
				exit(sem_komp);
			}
		default:
			E("valid: Semantic error - unsupported operation");
			datatypes(left->dataType, right->dataType);
			exit(sem_komp);
			break;
	}
	
	E("valid: Semantic error - undefined combination of operands and operator");
	datatypes(left->dataType, right->dataType);
	exit(sem_komp);
}

/**
 * Vytvari novy uzel do stromu podle operatoru.
 * --------------------
 * token: Urcuje typ jakeho by mel byl uzel AST
 * aststack: Urcuje zasobnik ze ktereho bude tahat data a kam bude ukladat vysledek
 */
int makeAstFromToken(struct toc* token, struct stack** aststack){
	//printTokenType(token);
	printAstStack(*aststack);

	struct astNode* node = makeNewAST();
	// vytazeny operator
	int prio = getPriority(token);
	if(prio > 0 && prio < 10){
		// operatory bez NOT
		node->type = convertTokenToNode(token);
		if((int)node->type == -1){
			E("Unsuported token type to convert");
			return False;
		}
		
		node->right = (struct astNode*)stackPop(*aststack);
		node->left = (struct astNode*)stackPop(*aststack);
		
		// kontrola validity operace a operandu
		//printTokenType(token);
		node->dataType = valid(node->left, node->right, node->type);
		switch(node->dataType){
			case DT_INT:
				D("maft: Valid operation by operand types - INTEGER");
				break;
			case DT_REAL:
				D("maft: Valid operation by operand types - REAL");
				break;
			case DT_STR:
				D("maft: Valid operation by operand types - STRING");
				break;
			case DT_BOOL:
				D("maft: Valid operation by operand types - BOOLEAN");
				break;
			case False:
				E("maft: Gets FALSE insted of data type");
				exit(sem_komp);
			default:
				E("maft: Invalid data");
				exit(sem_komp);
		}
	}
	else if(prio == 0){
		// not
		node->type = AST_NOT;
		node->left = (struct astNode*)stackPop(*aststack);
		if(!node->left){
			E("maft: Semantic error - stack is empty, no operand for NOT");
			exit(sem_komp);
		}
		
		if(node->left->dataType != DT_BOOL){
			E("maft: Semantic error - expected bool return type of operand");
			exit(sem_komp);
		}		
	}
	else {
		W("Stack error - invalid data");
		//printTokenType(token);
		return False;						
	}
	
	// ulozeni zpatky na zasobnik
	int ret = stackPush(*aststack, node);
	printAst((struct astNode*)stackTop(*aststack));
	return ret;
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
	W("parseProgram");
	struct toc* cur = getToc();

	if(cur->type != T_KW_PROGRAM){
		E("Syntax error - expected PROGRAM keyword");
		global.errno = synt;
		return NULL;
	}
	
	// nalezen start
	cur = getToc();
	if(cur->type != T_ID){
		// chyba, melo nasledovat oznaceni
		E("Syntax error - expected name of program");
		exit(synt);
	}
	
	// jmeno programu
	struct astNode* program = makeNewAST();
	program->type = AST_START;
	// kopirovani jmena programu
	if(!(copyString(cur->data.str, &(program->data.str))))
		return NULL;			
	//printString(program->data.str);
	
	cur = getToc();
	if(cur->type != T_SCOL){
		E("Syntax error - expected SEMICOLON");
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
	D("program: back from VARS");
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
	
	//printTokenType(cur);
	
	// nasleduje telo programu
	struct astNode* body = parseBody(&cur);		
	if(!body)
		return NULL;
	program->left = body;
	
	if(cur->type != T_DOT){
		E("program: Syntax error - expected END.");
		exit(synt);
	}
	
	cur = getToc();
	if(cur->type != T_EOF){
		E("program: Syntax error - expected program end");
		//printTokenType(cur);
		exit(synt);
	}
			
	return program;
}

/**
 * Parsuje telo, pouze zkontroluje zda-li zacina na begin a konci na end.
 * ---------------------------------------------------------------------
 */
struct astNode* parseBody(struct toc** cur){
	W("parseBody");
	
	if((*cur)->type != T_KW_BEGIN){ // nebyl nacten BEGIN
		(*cur) = getToc();
		
		if((*cur)->type != T_KW_BEGIN){
			E("body: Syntax error - expected BEGIN keyword");
			//printTokenType(*cur);		
			exit(synt);
		}
	}
	
	// zacatek tela -> spustit prikazy
	// ukladat je doleva za sebe do typu AST_CMD
	struct astNode* body = makeNewAST();
	struct astNode* cmd = parseCommand(cur);
	if((*cur)->type == T_KW_END){
		// ukonceni bloku kodu
		if(cmd == NULL){
			W("body: WARNING - statement without effect");
			
			body->left = NULL;
			return body;
		}
		
		body->left = makeNewAST();
		body->left->type = AST_CMD;
		body->left->left = NULL;
		body->left->right = cmd;
		
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
		
			// pokud narazi na END tak ukoncit hledani dalsich 
			if((*cur)->type == T_KW_END){
				(*cur) = getToc();
				break;
			}
			else if((*cur)->type == T_SCOL){
				// hledat dalsi
				cmd = parseCommand(cur);
			
				continue;
			}
			else {
				E("body: Syntax error - expected END or SEMICOLON");
				//printTokenType(*cur);
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
 * ---------------------------------
 */
struct astNode* parseVars(struct toc** cur){
	W("parseVars");
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
				//printString((*cur)->data.str);
			
				// odkladiste pro nove promenne do tabulky symbolu				
				struct symbolTableNode* new = NULL;
				
				(*cur) = getToc();
				if((*cur)->type == T_COL){
					//printTokenType(*cur);
					// dostal dvojtecku --> ocekavat typ
														
					(*cur) = getToc();
					switch((*cur)->type){
						case T_KW_INT: {
							var->type = AST_INT;
							
							// vytvorit novy zaznam v tabulce
							new = insertValue(&top, var->data.str, DT_INT);
								
							if(!new)
								return NULL;
							
							new->dataType = DT_INT;													
							
							stackPush(global.symTable, top);
							break;
						}
						case T_KW_REAL: {
							var->type = AST_REAL;
							
							new = insertValue(&top, var->data.str, DT_REAL);
							if(!new)
								return NULL;
								
							new->dataType = DT_REAL;
							
							stackPush(global.symTable, top);
							break;
						}
						case T_KW_BOOLEAN: {
							var->type = AST_BOOL;
											
							new = insertValue(&top, var->data.str, DT_BOOL);
							if(!new)
								return NULL;
								
							new->dataType = DT_BOOL;
							
							stackPush(global.symTable, top);
							break;
						}
						case T_KW_STRING: {
							var->type = AST_STR;
														
							new = insertValue(&top, var->data.str, DT_STR);
							if(!new)
								return NULL;
								
							new->dataType = DT_STR;
							
							stackPush(global.symTable, top);
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
									E("vars: Syntax error - expected integer as lower range of array index");
									//printTokenType(*cur);
									exit(synt);
								}
								dta->low = (*cur)->data.integer;
								
								// interval - dve tecky mezi integery
								(*cur) = getToc();
								if((*cur)->type != T_DDOT){
									E("vars: Syntax error - expected two dots");
									//printTokenType(*cur);
									exit(synt);
								}
								
								// ocekavat druhy integer
								(*cur) = getToc();
								if((*cur)->type != T_INT){
									E("vars: Syntax error - expected integer as higher range of array index");
									//printTokenType(*cur);
									exit(synt);
								}
								dta->high = (*cur)->data.integer;
								
								// ocekavam konec intervalu
								(*cur) = getToc();
								if((*cur)->type != T_RBRC){
									E("vars: Syntax error - expected right brace");
									//printTokenType(*cur);
									exit(synt);
								}
								
								// ocekavat OF
								(*cur) = getToc();
								if((*cur)->type != T_OF){
									E("vars: Syntax error - expected OF for defining type of array");
									//printTokenType(*cur);
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
										E("vars: Syntax error - unsupported type of array");
										//printTokenType(*cur);
										exit(synt);
								}
								// ulozeni odkazu na strukturu dat
								var->other = dta;		
								
								if(top == NULL){
									top = makeNewSymbolTable();
									stackPush(global.symTable, top);	
								}
								
								// vlozeni nazev pole do tabulky
								new = insertValue(&top, var->data.str, DT_ARR);
								if(!new)
									return NULL;
								
								// nastaveni ze se jedna o pole
								new->dataType = DT_ARR;					
								new->other = dta;								
							}
							else {
								E("vars: Syntax error - expected left brace");
								//printTokenType(*cur);
								exit(synt);
							}
							
							break;
						}
						default:
							E("vars: Syntax error - expected type");
							exit(synt);	
					}
				}				
				
				(*cur) = getToc();
				if((*cur)->type != T_SCOL){
					E("vars: Syntax error - expected semicolon after var deklaration");
					//printTokenType(*cur);
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
				E("vars: Syntax error - unsupported token");
				//printTokenType(*cur);
				exit(synt);			
			}
		}
		
		printSymbolTable(top, 0);
		
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
	W("parseParams");
	
	struct toc* cur = getToc();
	struct astNode* params = makeNewAST();
	if(!params)
		return NULL;
		
		
	if(cur->type != T_LPAR){
		E("params: Syntax error - expected Left parenthesis");
		//printTokenType(cur);
		exit(synt);	
	}
	
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	if(top == NULL)
		W("params: top layer is empty");
	
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
					E("params: Syntax error - expected COLON after identificator");
					//printTokenType(cur);
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
						E("params: Syntax error - unsupported type of parameter");
						//printTokenType(cur);
						exit(synt);
				}
				
				// ulozit do seznamu a pokracovat
				struct astNode* item = params;
				while(item->left != NULL)
					item = item->left;
				item->left = par;		
				
				insertValue(&top, par->data.str, par->dataType);	
				
				// ukonceni parametru
				cur = getToc();				
				if(cur->type == T_SCOL){
					// oddelovac parametru
					cur = getToc();
					continue;				
				}
				else if(cur->type == T_RPAR){
					// ukonceni parametru
					break;					
				}			
			}
			else {
				E("params: Syntax error - expected identificator of parameter");
				printTokenType(cur);
				exit(synt);	
			}
		
			cur = getToc();
		}
	}
	// pouze v pripade, ze byla zadana prava zavorka vratit seznam
	if(cur->type == T_RPAR){
		D("params: Printing current top layer of symbol table");		
		printSymbolTable(top, 0);
		stackPush(global.symTable, top);
		return params;	
	}
	return NULL;	
}

/**
 * Parsuje dopredne deklarace / definice funkci
 * --------------------------------------------
 */
struct astNode* parseFunction(){
	W("parseFunction");
	// definice funkce / dopredna definice --- FUNCTION je uz nactene
	// FUNCTION <id> <params>: <type>; <var_def> <body>     
	// FUNCTION <id> <params>: <type>; FORWARD;
	
	struct astNode* node = makeNewAST();
	node->type = AST_FUNC;
	
	struct toc* cur = getToc();
	if(cur->type != T_ID){
		E("function: Syntax error - expected ID after FUNCTION");
		//printTokenType(cur);
		exit(synt);
	}
	
	// skopirovani jmena
	if(!(copyString(cur->data.str, &(node->data.str))))
		return NULL;
	
	// sparsovat parametry
	struct symbolTableNode* newlayer;
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	copyTable(top, &newlayer);
	if(newlayer == NULL){
		E("function: copy failed");
		exit(intern);
	}
	stackPush(global.symTable, newlayer);
	node->right = parseParams();
	
	top = (struct symbolTableNode*)stackTop(global.symTable);
	
	cur = getToc();
	if(cur->type != T_COL){
		E("function: Syntax error - expected colon after function params");
		//printTokenType(cur);
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
			E("function: Syntax error - unsupported returning type of function");
			//printTokenType(cur);
			exit(synt);
		}
	}	
	// typ OK
	
	// ulozit do tabulky zastupnou promennou za return
	D("function: Inserting function name into symTable");
	insertValue(&top, node->data.str, node->dataType);
	printSymbolTable(top, 0);
	
	// prohledani tabulky funkci
	struct symbolTableNode* dekl = (struct symbolTableNode*)search(&(global.funcTable), node->data.str);
	
	cur = getToc();
	if(cur->type == T_KW_FORW){
		D("function: FORWARD DECLARATION");
		// dopredna deklarace - pokud ji najde ve funkcich je neco spatne
	 	if(!dekl)
		 	dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), node->data.str, node->dataType);
		else {
			// nalezl deklarovanou/definovanou funkci v tabulce - chyba!
			E("function: Syntax error - redefinition of forward declaration");
			exit(synt);
		}
			
		// v pripade dopredne deklarace neni nutne pouzivat parametry
		stackPop(global.symTable);
		top = (struct symbolTableNode*)stackTop(global.symTable);
		D("function: Printing FORWARD declaration symbol table");
		printSymbolTable(top, 0);
		
				
	 	// nastaveni navratovy typ
	 	dekl->dataType = node->dataType;
	 	// v node->right jsou parametry
	 	dekl->other = node;
	 	return node;
	}
	
	D("function: DEFINITION of function");
	
	// TODO vyresit definici tela a promennych funkce!
	// Pokud funkce bude nalezena - dopredna deklarace
	// 			pak nahrat telo a promenne do uzlu
	// Pokud funkce nebude nalezena - jedna se rovnou o definici funkce
	// 			vytvorit novy uzel v tabulce symbolu
	// 			pak nahrat telo a promenne do uzlu
	if(!(dekl)) {
		// nebyla nalezena - vlozit novy uzel
		if(!(dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), node->data.str, node->dataType)))
			return NULL;
	}
		
	if(dekl->other != NULL) {	
		if(((struct symbolTableNode*)dekl->other)->left != NULL){
			// telo uz bylo jednou definovane!!
			E("function: Syntax error - redefinition of function");
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
	
	// odstranit vrchol s parametry a return promennou
	D("function: Popping top layer for function definition");
	stackPop(global.symTable);			
	top = (struct symbolTableNode*)stackTop(global.symTable);
	D("function: Printing stable top layer after deleting parameters");
	printSymbolTable(top, 0);

	return node;
}

/**
 * Parsuje parametry volani funkce a vklada je do stromu, smerem doleva
 * --------------------------------------------------------------------
 */
struct astNode* parseCallParams(){
	W("parseCallParams");
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
				E("callPars: Syntax error - invalid parameter type");
				//printTokenType(cur);
				exit(synt);
		}
		
		// nacteni oddelovace
		cur = getToc();
		if(cur->type == T_COM){
			// nacetl carku - v poradku pokud nenasleduje T_RPAR
			cur = getToc();
			if(cur->type == T_RPAR){
				E("callPars: Syntax error - comma before right parenthesis");
				//printTokenType(cur);
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
	W("parseFuncCall");
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
		E("if: Syntax error - expected THEN keyword at the end of condition");
		//printTokenType(*cur);
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
 */
struct astNode* whileStatement(struct toc** cur){
	struct astNode* condition = parseExpression(cur);
	if(!condition)
		return NULL;
	
	if((*cur)->type != T_KW_DO){
		E("while: Syntax error - expected DO keyword at the end of condition");
		//printTokenType(*cur);
		exit(synt);
	}

	struct astNode* node = makeNewAST();				
	if(!node)
		return NULL;
	// ulozeni podminky
	node->other = condition;
	node->type = AST_WHILE;
	
	// kompletace tela
	node->left = parseBody(cur);
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
		E("repeat: Syntax error - repeat cycle must have at least one command in the body");
		exit(synt);
	}
	
	(*cur) = getToc();
	if((*cur)->type != T_UNTIL){
		E("repeat: Syntax error - expected UNTIL keyword");
		//printTokenType(*cur);
		exit(synt);
	}
	
	// nacetl UNTIL
	rpt->other = parseExpression(cur);
	if(!rpt->other)
		return NULL;
	
	return rpt;
}

/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * ------------------------------------------------------------------------------------
 */
struct astNode* parseCommand(struct toc** cur){
	W("parseCommand");
	(*cur) = getToc();
	
	//printTokenType(*cur);
	switch((*cur)->type){
		case T_KW_END: {
			// narazil hned na END -> po stredniku nasledoval END chyba
			E("cmd: Syntax error - last command cannot be ended with semicolon");
			exit(synt);
		}	
		case T_KW_IF: {			
			struct astNode* ifstat = ifStatement(cur);
			if(!ifstat)
				return NULL;
				
			if((*cur)->type == T_KW_END){			
				*cur = getToc();
				return ifstat;
			}
			else {
				E("cmd: Syntax error - expected END (if)");
				//printTokenType(*cur);
				exit(synt);
			}	
		}
		case T_KW_WHILE: {
			struct astNode* whl = whileStatement(cur);
			if(!whl)
				return NULL;
			
			if((*cur)->type == T_KW_END){
				*cur = getToc();
				return whl;			
			}
			else {
				E("cmd: Syntax error - expected END (while)");			
				//printTokenType(*cur);
				exit(synt);
			}
		}
		case T_RPT: {
			struct astNode* rpt = repeatStatement(cur);
			if(!rpt)
				return NULL;
			
			if((*cur)->type == T_KW_END){
				*cur = getToc();
				return rpt;
			}
			else {
				E("cmd: Syntax error - expected END (repeat)");
				//printTokenType(*cur);
				exit(synt);
			}
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
				D("cmd: cur and next token");
				printTokenType(*cur);
				printTokenType(next);
				
				// levy uzel je T_ID
				// pravy uzel je expression
				struct astNode* left = makeNewAST();
				if(!left)
					return NULL;					
				left->type = AST_ID;
				copyString((*cur)->data.str, &(left->data.str));
				D("cmd: After string copy")
				
				// skopirovani informaci z tabulky symbolu
				struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
				struct symbolTableNode* nd = search(&top, left->data.str);				
				if(!nd){
					E("cmd: Syntax error - variable not found");
					exit(synt);
				}
				left->dataType = nd->dataType;
				
				// prava strana je vyraz
				struct astNode* right = parseExpression(cur);
				if(!right) 
					return NULL;
				D("cmd: After expr parsing");
			
				if(left->dataType != right->dataType){
					E("cmd: Semantic error - L-value has not same value as R-value");
					exit(sem_komp);
				}
								
				// vytvorit uzel prirazeni a vratit jej
				struct astNode* asgn = makeNewAST();
				asgn->type = AST_ASGN;
				asgn->left = left;
				asgn->right = right;
				asgn->other = NULL;
				D("cmd: Returning asgn node");
				return asgn;
			}
			else {
				E("cmd: Syntax error - expected funcCall or assign");
				//printTokenType(*cur);
				exit(synt);
			}		
			break;
		}
	}	
	
	return NULL;
}

/**
 * Parsuje vyraz, aritmeticko-logicky
 * ---------------------------------
 * cur: odkaz na token z vyssi vrstvy
 */
struct astNode* parseExpression(struct toc** cur){
	W("parseExpression");
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// zasobnik pro chystani AST
	struct stack* aststack = makeNewStack();				
	
	// zacatek podminky
	(*cur) = getToc();
		
	// <id>(
	// <id> <operator> (
	if((*cur)->type == T_ID){
		D("expr: TOKEN ID on the left side");
	
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
				
			struct symbolTableNode* stable = (struct symbolTableNode*)stackTop(global.symTable);
			struct symbolTableNode* nd = (struct symbolTableNode*)search(&stable, node->data.str);
			
			node->dataType = nd->dataType;				
			node->left = NULL;
			node->right = NULL;
			
			//datatypes(nd->dataType, node->dataType);
			stackPush(aststack, node);
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
		D("expr: new token");
		////printTokenType(*cur);
		switch((*cur)->type){
			// leva zavorka
			case T_LPAR: {
				W("T_LPAR comes");
			
				if(!stackPush(stack, (*cur)))
					return NULL;
				break;
			}
			// prava zavorka
			case T_RPAR: {
				W("T_RPAR comes");
			
				struct toc* t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					E("expression: Syntax error - no Left parenthesis before Right parenthesis");
					exit(synt);
				}
				
				while(!stackEmpty(stack) && (t->type != T_LPAR)){
					//printTokenType(t);
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					if(!makeAstFromToken(t, &aststack)){
						E("expr: maft failed");
						return NULL;	
					}
					
					t = (struct toc*)stackPop(stack);				
					////printTokenType(t);
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
				W("Literal comes");
				//printTokenType(*cur);
				
				struct astNode* node = makeNewAST();
				
				if((*cur)->type == T_ID){
					node->type = AST_ID;
					// kopie jmena
					copyString((*cur)->data.str, &(node->data.str));
					
					// ziskani tabulky symbolu
					struct symbolTableNode* stable = (struct symbolTableNode*)stackTop(global.symTable);
					struct symbolTableNode* nd = search(&stable, node->data.str);
					D("expr: comparison between symtable node and ast node");
					datatypes(nd->dataType, node->dataType);
					
					node->dataType = nd->dataType;					
					D("expr: making ID");
				}
				else if((*cur)->type == T_INT){
					node->type = AST_INT;
					node->dataType = DT_INT;
					node->data.integer = (*cur)->data.integer;	
					
					D("expr: making INT");			
				}
				else if((*cur)->type == T_REAL){
					node->type = AST_REAL;
					node->dataType = DT_REAL;
					node->data.real = (*cur)->data.real;
					
					D("expr: making REAL");
				}
				else if((*cur)->type == T_BOOL){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = (*cur)->data.boolean;
					
					D("expr: making BOOL");
				}
				else if((*cur)->type == T_STR){
					node->type = AST_STR;
					if(!copyString((*cur)->data.str, &(node->data.str)))
						return NULL;
						
					node->dataType = DT_STR;
					D("expr: making STR");
				}
				else if((*cur)->type == T_KW_TRUE){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = True;
					
					D("expr: making TRUE");
				}
				else if((*cur)->type == T_KW_FALSE){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = False;
					
					D("expr: making FALSE");
				}
				else {
					D("expr: Unknown data type");
					exit(synt);
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
				W("Operator comes");
				//printTokenType(*cur);
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
			case T_KW_END:
			case T_KW_ENDDOT:
			case T_SCOL:
			case T_KW_DO:
			case T_KW_THEN: {
				// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
				D("expr: Making node into");
				//printTokenType(*cur);
				
				while(!stackEmpty(stack)){
					struct toc* now = (struct toc*)stackPop(stack);
					if(!makeAstFromToken(now, &aststack))
						return NULL;
						
					D("expr: stack state");
					printAstStack(stack);
				}
				
			
				// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
				if(aststack->Length > 1){
					E("expression: Shunting yard error - stack length is grather then 1");
					exit(intern);
				}
				else if(aststack->Length == 0){
					E("expression: Shunting yard error - stack is empty");
					exit(intern);
				}
			
				// vrati vrchol 
				D("expression: Returning top layer");
				return stackPop(aststack);
			}
			default:
				E("expression: Syntax error - bad token type");			
				//printTokenType(*cur);
				exit(synt);
		}
		
		// get next token
		(*cur) = getToc();
	}
	return NULL;
}
