#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Stack.h"
#include "Log.h"
#include "Ast.h"

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

/**
 * Vytvari novy uzel do stromu podle operatoru.
 * --------------------
 * token: Urcuje typ jakeho by mel byl uzel AST
 * aststack: Urcuje zasobnik ze ktereho bude tahat data a kam bude ukladat vysledek
 */
int makeAstFromToken(struct toc* token, struct stack** aststack){
	struct astNode* node = makeNewAST();
	// vytazeny operator
	int prio = getPriority(token);
	if(prio > 0 && prio < 10){
		// operatory bez NOT
		node->type = convertTokenToNode(token);
		if(node->type == -1){
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
		return False;						
	}
			
	return stackPush(*aststack, node);
}

/**
 * Parsuje vyraz, aritmeticko-logicky
 * --------------------
 */
struct astNode* parseException(){
	// vystupni strom
	struct astNode* ast = makeNewAST();
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// fronta pro vystupni postfix zapis
	struct queue* outqueue = makeNewQueue();
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
		switch(toc->type){
			// leva zavorka
			case T_LPAR: {
				if(!stackPush(stack, toc))
					return NULL;
				break;
			}
			// prava zavorka
			case T_RPAR: {
				struct toc* t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					Log("Syntax error - no '(' before ')'", ERROR, PARSER);
					return NULL;
				}
			
				struct toc* helper;
				while((t = (struct toc*)stackPop(stack)) != NULL && t != T_LPAR){
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					
					if(!makeAstFromToken(t, &aststack))
						return NULL;					
				}
				
				break;
			}
			// operandy
			case T_ID:
			case T_INT:
			case T_REAL:
			case T_STR:
			case T_BOOL: {
				struct astNode* node = makeNewAST();
				
				if(toc->type == T_ID){
					node->type = AST_ID;
					// kopie jmena
					if(!copyString(toc->data.str, &(node->data.str)))
						return NULL;
				}
				else if(toc->type == T_INT){
					node->type = AST_INT;
					node->data.integer = toc->data.integer;				
				}
				else if(toc->type == T_REAL){
					node->type = AST_REAL;
					node->data.real = toc->data.real;
				}
				else if(toc->type == T_BOOL){
					node->type = AST_BOOL;
					node->data.boolean = toc->data.boolean;
				}
				else if(toc->type == T_STR){
					node->type = AST_STR;
					if(!copyString(toc->data.str, &(node->data.str))
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
				// 3. operator:
				// 		pokud je zasobnik prazdny
				// 		pokud je na vrcholu leva zavorka
				// 		pokud je na vrcholu operator s nizsi prioritou
				// pokud je na vrcholu operator s vyssi prioritou -> 
				// 		presunout na vystup a opakovat 3. krok
			
				struct toc* top;
				if(stackEmpty(stack)){
					if(!stackPush(stack, toc))
						return NULL;
				}
				else {
					while(!stackEmpty(stack)){			
						top = stackTop(stack);
						if(top == NULL){
							if(!stackPush(stack, toc))
								return NULL;
						
							// vlozen prvek do zasobniku
							break;
						}
								
						// v pripade, ze je na vrcholu vyssi priorita				
						if(getPriority(top) >= getPriority(toc)){
							// vybrat a poslat na vystup a opakovat
							top = stackPop(stack);
						
							if(!makeAstFromToken(top, &aststack))
								return NULL;
						}
								
						// pokus vlozeni - za jakekoliv okolnosti 
						if(stackEmpty(stack) || top->type == T_LPAR || (getPriority(top) < getPriority(toc))){
							if(!stackPush(stack, toc))
								return NULL;
							break;
						}
					}		
				}	
				break;
			}
			// ukoncovaci operatory/klicove slova/separatory
			case T_THEN: 
			case T_DO:
			case T_SCOL: {
				// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
				while(!stackEmpty(stack)){
					if(!makeAstFromToken(stackPop(stack), &aststack))
						return NULL;
				}
				
				// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
				if(aststack->Length == 1)
					return stackPop(aststack);
				else {
					Log("AST stack contains more than one item!", ERROR, PARSER);
					return NULL;
				}
					
				break;
			}
			default:
				Log("Syntax error - invalid token type", ERROR, PARSER);	
				return NULL;	
		}
		
		// get next token
		cur = getToc();
	}
}


















