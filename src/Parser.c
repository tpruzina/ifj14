#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Stack.h"
#include "Log.h"
#include "Ast.h"
#include "Scanner.h"
#include "Parser.h"

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
	{"T_EOF", T_EOF}};
	
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
		
		Log("Dyadic operators", WARNING, PARSER);
		node->right = (struct astNode*)stackPop(*aststack);
		node->left = (struct astNode*)stackPop(*aststack);
	}
	else if(prio == 0){
		// not
				
		Log("Not operator", WARNING, PARSER);
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

struct astNode* parseParams(){
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
				//TODO vytvorit ast node a vlozit doleva nakonec
				
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
				
				nd->type = AST_BOOLEAN;
				nd->data.boolean = cur->data.boolean;
				
				break;
			}
			case T_STR: {
				// predany parametry typu string
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_STR;
				
		
				break;
			}
			case T_ARR: {
				// predane pole...
			
			
				break;
			}
			default:
				Log("Syntax error - invalid parameter type", ERROR, PARSER);
				return NULL;
		}
		
		//TODO vytvoreni polozky a vlozeni polozky do node
		
		
		// nacteni dalsiho tokenu
		cur = getToc();
	}
	
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
	
	
	if(!(node->left = parseParams()))
		return NULL;

	node->right = NULL;
	return NULL;
}

/**
 * Parsuje telo, pouze zkontroluje zda-li zacina na begin a konci na end.
 * --------------------
 */
struct astNode* parseBody(){
	struct toc* cur = getToc();
		
	if(cur->type == T_KW_BEGIN){
		//	zacatek tela -> spustit prikazy
		struct astNode* body = parseCommands();
		if(!body)
			return NULL;
			
		// nacteni konce END
		cur = getToc();
		if(cur->type == T_KW_END)
			return body;
		else {
			Log("Syntax error - expected END token at the end of body", ERROR, PARSER);
			return NULL;
		}
	}
	else {
		Log("Syntax error - expected BEGIN token", ERROR, PARSER);
		return NULL;
	}
}
/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * --------------------
 * TODO Volani funkci apod.
 */
struct astNode* parseCommand(){
	struct toc* cur = getToc();
	
	switch(cur->type){
		case T_KW_IF: {
			struct astNode* ifnode;
		
			// nasleduje podminka
			struct astNode* condition = parseException(T_KW_THEN);
			if(!condition)
				return NULL;
				
			if(!(fnode = makeNewAST()))
				return NULL;
			ifnode->othen = condition;
			
			// telo true
			ifnode->left = parseBody();						
			// telo else -- 
			cur = getToc();
			if(cur->type == T_KW_ELSE)
				ifnode->right = parseBody();
			else if(cur->type == T_END){
				// kratky if -> musi nasledovat strednik
				cur = getToc();
				if(cur->type == T_SCOL){
					// validni kratky if
				
					ifnode->right = NULL;
					return ifnode;
				}
				else {
					// chybi strednik!!
					// TODO z konzultace zjistit jestli ma byt nebo ne
				}					
			}		
			else {
				Log("Syntax error - expected ELSE or END", ERROR, PARSER);
				return NULL;		
			}
			break;
		}
		case T_KW_WHILE: {
			struct astNode* node;
			
			struct astNode* condition = parseException(T_KW_DO);
			if(!condition)
				return NULL;
				
			if(!(node = makeNewAST()))
				return NULL;
			// ulozeni podminky
			node->other = condition;
			
			// kompletace tela
			if((node->left = parseBody()) == NULL)
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
				
			// zjisteni tela
			if(!(node->left = parseBody()))
				return NULL;
			
			cur = getToc();
			if(cur->type == T_KW_UNTIL){
				// korektni pokracovani
				
				// precteni podminky
				if(!(node->other = parseException(T_SCOL)))
					return NULL;
				
				node->right = NULL;
				return node;
			}
			else {
				Log("Syntax error - expected UNTIL", ERROR, PARSER);
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
				
			}
			else if(next->type == T_ASG){
				// pravdepodobne prirazeni
				
				// levy uzel je T_ID
				// pravy uzel je expression
				struct astNode* left = makeNewAST();
				if(left == NULL)
					return NULL;
					
				left->type = AST_ID;
				if(!copyString(cur->data.str, &(left->data.str)))
					return NULL;
				
				// prava strana je vyraz
				struct astNode* right = parseExpression(T_SCOL);
				if(right == NULL) 
					return NULL;
				
				// vytvorit uzel prirazeni a vratit jej
				struct astNode* asgn = makeNewAST();
				asgn->type = AST_ASGN;
				asgn->left = left;
				asgn->right = right;
				return asgn;
			}
			else {
				Log("Syntax error - expected funcCall or assign", ERROR, PARSER);
				return NULL;
			}		
			break;
		}
	}	
}

/**
 * Parsuje vyraz, aritmeticko-logicky
 * --------------------
 * TODO Doplnit rozliseni volani funkce!!!
 */
struct astNode* parseException(int endToken){
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// zasobnik pro chystani AST
	struct stack* aststack = makeNewStack();				
	// zacatek podminky
	struct toc* cur = getToc();
	
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
					Log("Syntax error - no '(' before ')'", ERROR, PARSER);
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
					/*
					while(!empty($stack) && ($top->prior >= $token->prior)){
						array_push($out, array_pop($stack));
						
						$top = array_top($stack);
					}
					array_push($stack, $token);					
					*/
				
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
			// ukoncovaci operatory/klicove slova/separatory
			case endToken: {
				Log("End marks comes", WARNING, PARSER);
				// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
				while(!stackEmpty(stack)){
					struct toc* now = (struct toc*)stackPop(stack);
					if(!makeAstFromToken(now, &aststack))
						return NULL;
				}
				
				// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
				
				return stackPop(aststack);
			}
			default:
				Log("Syntax error - invalid token type", ERROR, PARSER);	
				return NULL;	
		}
		
		// get next token
		cur = getToc();
	}
	return NULL;
}


















