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
		return 10;
	}

	switch(t->type){
		case T_EQV:
		case T_NEQV:
		case T_GRT:
		case T_LSS:
		case T_GEQV:
		case T_LEQV:
			return 3;
		case T_ADD:
		case T_SUB:
		case T_OR:
		case T_XOR:
			return 2;
		case T_MUL:
		case T_DIV:
		case T_AND:
			return 1;
		case T_NOT:
			return 0;
		default:
			return 10;	
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


/**
 * Parsuje vyraz, aritmeticko-logicky
 * --------------------
 */
struct astNode* parseException(){
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
				while(!stackEmpty(stack) && t->type != T_LPAR){
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
			case T_BOOL: {
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
					while(!stackEmpty(stack)){			
						top = stackTop(stack);
						if(top == NULL){
							if(!stackPush(stack, cur))
								return NULL;
						
							// vlozen prvek do zasobniku
							break;
						}
								
						// v pripade, ze je na vrcholu vyssi priorita
						if(getPriority(top) >= getPriority(cur)){
							// vybrat a poslat na vystup a opakovat
							top = stackPop(stack);

							//TODO tady se sere LPAR, i kdyz by mela byt v prdeli pryc
							if(!makeAstFromToken(top, &aststack))
								return NULL;
						}						
						
						// pokus vlozeni - za jakekoliv okolnosti 
						if(stackEmpty(stack) || top->type == T_LPAR || (getPriority(top) < getPriority(cur))){
							if(!stackPush(stack, cur))
								return NULL;
							break;
						}
					}		
				}	
				break;
			}
			// ukoncovaci operatory/klicove slova/separatory
			case T_KW_THEN: 
			case T_KW_DO:
			case T_SCOL: {
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


















