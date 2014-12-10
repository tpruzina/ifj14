/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativnÃ­ho jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antoní­n    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juřík Martin     <xjurik08@stud.fit.vutbr.cz           ****
****               David Petr       <xdavid15@stud.fit.vutbr.cz>          ****
****                                                                      ****
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "stack.h"
#include "log.h"
#include "ast.h"
#include "scanner.h"
#include "parser.h"
#include "ial.h"

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
		return -1;
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
			return -1;	
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
	{"T_KW_RPT",T_KW_RPT},
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
	if(!PRT) return;
	for(int i=0; tokennames[i].str; i++){
		if((int)token->type == (int)tokennames[i].type){
			fprintf(stderr, "Token: %s\n", tokennames[i].str);
			
			if((int)token->type == T_ID){
				fprintf(stderr, "\t%s\n", token->data.str->Value);
			}
		}
	}
}

int expect(struct toc* cur, int type, int exitcode){
	if((int)cur->type != type){
		char str[50];
		char toctype[50];
		char exptype[50];
		
		for(int i=0; tokennames[i].str; i++){
			if((int)cur->type == tokennames[i].type)
				strcpy(toctype, tokennames[i].str);
			
			if(type == tokennames[i].type)
				strcpy(exptype, tokennames[i].str);
		}
		
		sprintf(str, "Syntax error - expected %s, gets %s", exptype, toctype);
		E(str);
		
		if(exitcode > 0)
			exit(exitcode);
			
		return false;
	}

	return true;
}

/**
 * Pomocna funkce, pro hledani promennych v TOP vrstve tabulky symbolu
 */
struct symbolTableNode* searchOnTop(struct String* name){
	struct symbolTableNode* nd = NULL;
	
	// nejprve hledat v globalni tabulce
	// mimo funkci je NULL, takze preskoci
	if(global.globalTable){
		nd = search(&global.globalTable, name);
	}
	
	// pokud je stale NULL
	if(!nd){
		struct symbolTableNode* stable = (struct symbolTableNode*)stackTop(global.symTable);
		nd = (struct symbolTableNode*)search(&stable, name);
		if(!nd){
			// nedefinovana promenna
			E("searchOnTop: semantic error - undefined variable");
			exit(sem_prog);
		}
	}
	
	return nd;
}

void printAstStack(struct stack* aststack){
	if(!PRT) return;

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

void printVarsPars(struct varspars* vp){
	if(!PRT) return;
	
	struct queueItem* item;
	// vars
	if(vp->vars != NULL){
		fprintf(stderr, "%s===== Vars queue =====%s\n", COLOR_LBLU, COLOR_NRM);
		item = vp->vars->start;
		while(item != NULL){
			printAst((struct astNode*)item->value);
		
			item = item->next;
		
			if(item != NULL)
				fprintf(stderr, "%s======================%s\n", COLOR_LBLU, COLOR_NRM);
		}
		fprintf(stderr, "%s=== Vars queue END ===%s\n", COLOR_LBLU, COLOR_NRM);
		}
	// pars
	if(vp->pars != NULL){
		fprintf(stderr, "%s===== Pars queue =====%s\n", COLOR_LCYN, COLOR_NRM);
		item = vp->pars->start;
		while(item != NULL){
			printAst((struct astNode*)item->value);
		
			item = item->next;
		
			if(item != NULL)
				fprintf(stderr, "%s======================%s\n", COLOR_LCYN, COLOR_NRM);
		}
		fprintf(stderr, "%s=== Pars queue END ===%s\n", COLOR_LCYN, COLOR_NRM);
	}
}

void datatypes(int left, int right){
	const char *l = NULL;
	const char *r = NULL;
	
	switch(left){
		case DT_INT:
			l = "DT_INT";
			break;
		case DT_REAL:
			l = "DT_REAL";
			break;
		case DT_BOOL:
			l = "DT_BOOL";
			break;
		case DT_STR:
			l = "DT_STR";
			break;
		case DT_NONE:
			l = "DT_NONE";
			break;
		case DT_ARR:
			l = "DT_ARR";
			break;
		default:
			l = "??????";
			break;
	}
	
	switch(right){
		case DT_INT:
			r = "DT_INT";
			break;
		case DT_REAL:
			r = "DT_REAL";
			break;
		case DT_BOOL:
			r = "DT_BOOL";
			break;
		case DT_STR:
			r = "DT_STR";
			break;
		case DT_NONE:
			r = "DT_NONE";
			break;
		case DT_ARR:
			r = "DT_ARR";
			break;
		default:
			l = "??????";
			break;
	}
#ifdef _DEBUG
	fprintf(stderr, "%s< Comparison between %s and %s >%s\n", ((right==left)?COLOR_LGRN:COLOR_LRED), l, r, COLOR_NRM);	
#endif
}

int valid(struct astNode* left, struct astNode* right, int op){	
	if(op == AST_NOT){
		if(left == NULL){
			E("valid: left node for NOT operator may not be NULL");
			exit(sem_prog);
		}
	}
	else {
		// binarni operace		
		if(left == NULL || right == NULL){
			E("valid: NULL operands");
			exit(synt);
		}
	}
	
	// dodatecne typy leve strany
	if(left->type == AST_ID){
		struct symbolTableNode* id = searchOnTop(left->data.str);
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
		struct symbolTableNode* id = searchOnTop(right->data.str);
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
void makeAstFromToken(struct toc* token, struct stack** aststack){
	D("makeAstFromToken");
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
			exit(synt);
		}
		
		node->right = (struct astNode*)stackPop(*aststack);
		node->left = (struct astNode*)stackPop(*aststack);
		
		// kontrola validity operace a operandu
		//printTokenType(token);
		D("Validity");
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
		exit(synt);						
	}
	
	D("Printing ast");
	// ulozeni zpatky na zasobnik
	stackPush(*aststack, node);
	printAst((struct astNode*)stackTop(*aststack));
}

/**
 * Tiskne obsah zasobniku tabulky symbolu
 * --------------------------------------
 */
void printSymbolTableStack(){
#ifdef _DEBUG
	fprintf(stderr, "=====================  STACK  ====================\n");

	struct stackItem* item = global.symTable->Top;
	while(item != NULL){
		struct symbolTableNode* top = (struct symbolTableNode*)item->Value;
	
		printSymbolTable(top, 0);
		
		item = item->Next;
		if(item != NULL)
			fprintf(stderr, "==================================================\n");
	}

	fprintf(stderr, "===================  END STACK  ==================\n");
#endif
}

int makeDataType(struct toc* cur){
	switch(cur->type){
		case T_KW_INT:
			return DT_INT;
		case T_KW_REAL:
			return DT_REAL;
		case T_KW_BOOLEAN:
			return DT_BOOL;
		case T_KW_STRING:
			return DT_STR;
		default:
			E("Syntax error - expected supported data type");
			exit(synt);
	}
	
	return DT_NONE;
}

/**
 * Hlavni funkce parseru - spousti rekurzivni pruchod
 * --------------------
 */
int parser(){
	//struct stackItem* top = (struct stackItem*) gcMalloc(sizeof(struct stackItem));
	//top->Value = makeNewSymbolTable();
	//global.symTable->Top = top;
	global.symTable->Top = NULL;
	
	D("PARSER START");
	
	struct astNode* prog = parseProgram();
	if(!prog){
		E("parser: Ended with false");
		return False;
	}
	
	// prohledat celou tabulku funkci a hledat polozku bez tela
	checkFunctionDeclarations(global.funcTable);
	
	global.globalTable = NULL;
	
	global.program = prog;
	return True;
}

int checkFunctionDeclarations(struct symbolTableNode* gft){
	// pokud predana tabulka neni prazdna
	if(gft == NULL){
		return False;
	}
	
	// telo funkce
	if(gft->init == false){
		E("Function without ast node deklaration");
		exit(sem_prog);
	}
	
	struct astNode* telo = (struct astNode*)gft->other;
	// hledani posloupnosti prikazu v levem podstromu
	if(telo->left == NULL){
		E("Function without body definition");
		printString(gft->name);		
		exit(sem_prog);
	}
	
	// prohledavani poduzlu
	if(gft->left != NULL)
		return checkFunctionDeclarations(gft->left);
	if(gft->right != NULL)
		return checkFunctionDeclarations(gft->right);
		
	return True;
}

/**
 * Kontroluje zacatek programu, existenci tokenu program, 
 * jmeno programu a nasledne telo
 * -----------------------------------------------------
 */
struct astNode* parseProgram(){
	W("parseProgram");
	
	struct astNode* program = makeNewAST();
	program->type = AST_START;

	struct toc* cur = getToc();

	// ok -> nasleduje telo programu nebo definice promennych nebo definice 
	// funkci/dopredne definice funkci			
	// pokud bude nasledujici token BEGIN pak ctu telo
	// pokud bude nasledujici VAR pak jsou to lokalni
	if(cur->type == T_KW_VAR){
		struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
		vp->vars = parseVars(&cur);
		// lokalni promenne
		program->other = vp;
		printVarsPars(vp);
	}
	
	// prochazet definice funkci a vkladat je s telem do tabulky symbolu
	while(cur->type == T_KW_FUNC){
		D("Another function");
		struct astNode* func = parseFunction();				
		if(!func)
			return NULL;
		printAst(func);
		
		// muze nacitat 
		cur = getToc();		
	}
		
	expect(cur, T_KW_BEGIN, synt);
	// nasleduje telo programu
	struct astNode* body = parseBody(&cur, true, T_KW_END);		
	if(!body)
		return NULL;
	program->left = body;

	// predpoklad ze posledni token bude end
	D("program: after body");
	printTokenType(cur);	
	
	cur = getToc();
	expect(cur, T_DOT, synt);
	
	cur = getToc();
	expect(cur, T_EOF, synt);
			
	return program;
}

/**
 * Parsuje telo, pouze zkontroluje zda-li zacina na begin a konci na end.
 * ---------------------------------------------------------------------
 */
struct astNode* parseBody(struct toc** cur, bool empty, int endtype){
	W("parseBody");
	
	// zacatek tela -> spustit prikazy
	// ukladat je doleva za sebe do typu AST_CMDparseBody
	struct astNode* body = makeNewAST();
	body->type = AST_CMD;
		
	*cur = getToc(); // prvni token tela!!!
	printTokenType(*cur);
	if((int)(*cur)->type == endtype){
		D("Empty body");	
		if(empty)
			return body;
		else {
			E("Syntax error - empty body is not supported in this case");
			exit(synt);		
		}
	}

	D("Before command while");
	printTokenType(*cur);

 	struct astNode* cmd = parseCommand(cur);
	if((int)((*cur)->type) == endtype){
		D("body: gets END");
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
		D("body: gets SEMICOLON");
		
		struct astNode* item = body;
		// ocekavat dalsi command
		while(cmd != NULL){
			item->left = makeNewAST();
			item->left->type = AST_CMD;
			item->left->left = NULL;
			item->left->right = cmd;
		
			D("Waiting for end or scol");
			printTokenType(*cur);
			// pokud narazi na END tak ukoncit hledani dalsich 
			if((int)((*cur)->type) == endtype){
				D("Gets endtype");
				break;
			}
			else if((*cur)->type == T_SCOL){
				// nutno nacist dalsi token
				*cur = getToc();
				printTokenType(*cur);
				// hledat dalsi
				cmd = parseCommand(cur);
				// posun na dalsi pole v seznamu prikazu
				item = item->left;
			}
			else {
				E("body: Syntax error - expected END or SEMICOLON");
				printTokenType(*cur);
				exit(synt);			
			}			
		}
	}

	W("parseBody end");
	printAst(body);
	return body;
}

struct astNode* getArrayDef(struct toc** cur){
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);

	struct astNode* var = makeNewAST();
	struct dataTypeArray* dta;
	// odkladiste pro nove promenne do tabulky symbolu				
	struct symbolTableNode* new = NULL;
	
	// typ pole
	var->type = AST_ID;
	var->dataType = DT_ARR;

	dta = (struct dataTypeArray*)gcMalloc(sizeof(struct dataTypeArray));
	if(!dta)
		return NULL;
	copyString(var->data.str, &(dta->id));
	
	(*cur) = getToc();
	if(expect((*cur), T_LBRC, synt)) {
		// leva [ -> ocekavat integer								
		(*cur) = getToc();
		expect(*cur, T_INT, synt);
		dta->low = (*cur)->data.integer;
		
		// interval - dve tecky mezi integery
		(*cur) = getToc();
		expect(*cur, T_DDOT, synt);
		
		// ocekavat druhy integer
		(*cur) = getToc();
		expect(*cur, T_INT, synt);
		dta->high = (*cur)->data.integer;
		
		// ocekavam konec intervalu
		(*cur) = getToc();
		expect(*cur, T_RBRC, synt);
		
		// ocekavat OF
		(*cur) = getToc();
		expect(*cur, T_OF, synt);
		
		// nacteni typu promenne
		(*cur) = getToc();
		dta->type = makeDataType(*cur);
		
		// ulozeni odkazu na strukturu dat
		var->other = dta;		
									
		// vlozeni nazev pole do tabulky
		new = insertValue(&top, var->data.str, DT_ARR);
		if(!new)
			return NULL;
		
		// nastaveni ze se jedna o pole
		new->dataType = DT_ARR;					
		new->other = dta;								
	}
	return var;
}

/*
 *	Ocekava nacteni ID z definici
 */
struct astNode* getVarDef(struct toc** cur){
	// id	
	expect(*cur, T_ID, synt);
	
	// novy uzel + kopie jmena
	struct astNode* node = makeNewAST();
	node->type = AST_ID;
	copyString((*cur)->data.str, &(node->data.str));

	// ocekavat dvojtecku
	*cur = getToc();
	expect(*cur, T_COL, synt);
	
	// nacteni datoveho typu
	*cur = getToc();
	node->dataType = makeDataType(*cur);
	if(node->dataType == DT_NONE){
		E("Syntax error - unexpected token type");
		exit(synt);
	}
	
	// kazda promenna musi koncit strednikem
	*cur = getToc();
	expect(*cur, T_SCOL, synt);
	
	return node;
}

/**
 * Parsuje var sekci funkce/programu
 * ---------------------------------
 */
struct queue* parseVars(struct toc** cur){
	W("parseVars");

	// zacinat parsovat promenne
	struct queue* vars = makeNewQueue();		
	struct symbolTableNode* top = (struct symbolTableNode*)stackPop(global.symTable);
	
	struct astNode* var = NULL;
	*cur = getToc();
	expect(*cur, T_ID, synt);
	while((*cur)->type == T_ID){
		// nacteni definice
		var = getVarDef(cur);					
						
		// vlozit prvek
		queuePush(vars, var);
		
		// ulozit do top vrstvy
		if(search(&global.globalTable, var->data.str)){
			E("Promenna nalezena v globalni tabulce");
			exit(sem_prog);
		}
		else
			insertValue(&top, var->data.str, var->dataType);
		
		// nacist dalsi token
		*cur = getToc();
	}
		
#ifdef _DEBUG	
	printSymbolTable(top, 0);		
#endif

	stackPush(global.symTable, top);
			
	// vratit seznam promennych
	return vars;
}



/**
 *	Pasruje parametry definice funkce, podle tabulky.
 */
struct astNode* getDefPar(struct toc** cur){
	// parsuje def_par pouze:
	// id:type
	
	struct astNode* par = makeNewAST();
	par->type = AST_ID;
	
	// nacteni prvniho tokenu
	*cur = getToc();
	printTokenType(*cur);
	
	// konec parametru
	if((*cur)->type == T_RPAR)
		return NULL;
	
	// jinak ocekavat ID
	expect(*cur, T_ID, synt);
	
	// ulozeni jmena parametru
	copyString((*cur)->data.str, &(par->data.str));
	
	// ocekavani dvojtecky
	*cur = getToc();
	expect(*cur, T_COL, synt);
	
	// nacteni datoveho typu
	*cur = getToc();
	par->dataType = makeDataType(*cur);
	
	D("<Printing parametr>");
	printAst(par);
	D("</Printing parametr>");
	return par;
}

/**
 * Parsuje parametry v definici/deklaraci funkce
 * ---------------------------------------------
 */
struct queue* parseParams(){
	W("parseParams");
	struct toc* cur = NULL;
	struct queue* params = makeNewQueue();
	if(!params)
		return NULL;
			
	// nelezl levou zavorku -> nacitat dokud nenarazi na pravou zavorku
	struct astNode* par = getDefPar(&cur);
	// nacitani dokud jsou parametry
	while(par != NULL){	
		// nebyla nalezena pridat do vrstvy	pro funkce!!
		insertValue(&global.globalTable, par->data.str, par->dataType);
					
		// pridani parametru do fronty
		queuePush(params, par);
		
		// nacist separator
		cur = getToc();
		if(cur->type == T_SCOL){
			// ocekavat dalsi parametr
			par = getDefPar(&cur);
		}
		else if(cur->type == T_RPAR){
			// konec parametru
			break;
		}
		else {
			// za parametrem mohou byt jen 2 typy tokenu
			E("Syntax error - unsupported token in parameter definition");
			exit(synt);
		}
	}
		
	W("/parseParams");
	// pouze v pripade, ze byla zadana prava zavorka vratit seznam
	
	return params;		
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
	
	// nacteni prvniho tokenu za FUNCTION
	struct toc* cur = getToc();
	// ocekavani nazvu
	expect(cur, T_ID, synt);
	
	// skopirovani jmena
	struct String* name = makeNewString();
	copyString(cur->data.str, &name);
	node->other = name;

	// nacteni leve zavorky na zacatek parametru
	cur = getToc();
	expect(cur, T_LPAR, synt);

	//struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	
	// vyhodnoti parametry a vytvori frontu
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->pars = parseParams();
		
	// za parametry nasleduje dvojtecka
	cur = getToc();
	expect(cur, T_COL, synt);
	
	// za dvojteckou datovy typ navratove promenne
	cur = getToc();
	node->dataType = makeDataType(cur);
	
	// za datovym typem nasleduje strednik
	cur = getToc();
	expect(cur, T_SCOL, synt);
	
	// ulozit do tabulky zastupnou promennou za return spolu s parametry
	insertValue(&global.globalTable, name, node->dataType);
	D("Before deklaration");
	printSymbolTable(global.globalTable, 0);
	
	///////////////////////////////////////////
	//	KONEC HLAVICKY FUNKCE
	///////////////////////////////////////////
	// deklarace funkce
	struct symbolTableNode* dekl = (struct symbolTableNode*)search(&global.funcTable, name);
	if(dekl){
		// nalezl zaznam
		D("Function is in funcTable");
		if(dekl->init){
			D("Function has definition -- REDEFINITION");
		}
		else {
			D("Function has declaration -- MAY BE REDEFINITION");
		}
		
		if(dekl->other){
			D("Function has body node");
		}
		else {
			D("Function has not body node");
		}
	}
		
	// -------------------------
	// DOPREDNA DEKLARACE FUNKCE
	// -------------------------
	cur = getToc();
	if(cur->type == T_KW_FORW){
		D("FORWARD DEC!");
		printString(name);
				
		cur = getToc();
		// za FORWARD ocekavat strednik
		expect(cur, T_SCOL, synt);
	
	
		// dopredna deklarace - pokud ji najde ve funkcich je neco spatne
	 	if(dekl == NULL) // nenasel je to OK
		 	dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), name, node->dataType);
		else {
			// nalezl deklarovanou/definovanou funkci v tabulce
			// neni mozne znovu deklarovat
			E("function: Semantic error - function redefinition");
			exit(sem_prog);
		}
			
		// v pripade dopredne deklarace neni nutne pouzivat parametry
		//stackPop(global.symTable);
		deleteTable(&global.globalTable);
						
	 	// nastaveni navratovy typ
	 	dekl->dataType = node->dataType;
	 	// jedna se o deklaraci - tedy init = false
	 	dekl->init = false;
	 		 	
	 	// pravy uzel s parametry funkce
	 	node->right = makeNewAST();
	 	node->right->other = vp;
	 	D("Printing ast node");
	 	printAst(node);	 	

	 	dekl->other = node;
	 	D("FORWARD declaration");
	 	
#ifdef _DEBUG
	 	printSymbolTable(dekl,0);
#endif
	 	return node;
	}
	
	// ----------------------
	// DEFINICE OBSAHUJE TELO - snad
	// ----------------------
	if(cur->type == T_KW_VAR || cur->type == T_KW_BEGIN){
		if(!dekl) {
			// nebyla nalezena - vlozit novy uzel
			dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), name, node->dataType);
		}
		
		if(dekl->init == true){
			E("Semantic error - function redefinition");
			exit(sem_prog);
		}
		else {			
			// v pripade ze mel DEKLARACI nebo vznikla nova polozka
			struct astNode* telo = (struct astNode*)(dekl->other);	
			if(telo){		
				// mel deklaraci - asi
				struct astNode* vpnode = telo->right;
				if(!vpnode){
					E("Right varspars subnode is empty");
					exit(intern);
				}
			
				// kontrola na spravnost parametru s deklaraci
				struct varspars* deklpars = (struct varspars*)vpnode->other;
				controlDefinitionParams(vp->pars, deklpars->pars);
			 	D("Control def params DONE");
				if(telo->left != NULL){
					// telo uz bylo jednou definovane!!
					E("function: Syntax error - redefinition of function");
					exit(sem_prog);
				}
			}
		}
	
		if(cur->type == T_KW_VAR){
			// ocekavat var-def --> other
			vp->vars = parseVars(&cur);
			D("parseVars end");
			// po definici promennych MUSI nasledovat telo, tedy begin
			expect(cur, T_KW_BEGIN, synt);
		}
		node->right = makeNewAST();
		node->right->other = vp;
		printVarsPars(vp);
		
		// funkce byla definovana - tedy je inicializovana
		// hack pro podporu rekurze
		dekl->init = true;
		// ulozeni do tabulky symbolu
		dekl->other = node;	
		
		// ocekavat telo
		D("Body expectation");
		node->left = makeNewAST();
		node->left = parseBody(&cur, false, T_KW_END);
		
		// nacist strednik za definici
		cur = getToc();
		expect(cur, T_SCOL, synt);
		
		// odstranit vrchol s parametry a return promennou
		//stackPop(global.symTable);			
		deleteTable(&global.globalTable);
		return node;
	}
	
	E("function: Syntax error - definition of function without body");
	exit(synt);
}

/**
 * Parsuje parametry volani funkce a vklada je do stromu, smerem doleva
 * --------------------------------------------------------------------
 */
struct queue* parseCallParams(struct toc** cur){
	W("parseCallParams");
	// prochazi podel parametru a vytvari doleva jdouci strom z parametru
	struct queue* pars = makeNewQueue();
	
	(*cur) = getToc();
	while((*cur)->type != T_RPAR){
		struct astNode* nd = makeNewAST();
		printTokenType(*cur);
		struct symbolTableNode* id;
		switch((*cur)->type){
			case T_ID:
				// predana promenna
				
				id = searchOnTop((*cur)->data.str);
				nd->type = AST_ID;
				copyString((*cur)->data.str, &(nd->data.str));
				nd->dataType = id->dataType;
				datatypes(nd->dataType, id->dataType);
				
				break;
			case T_INT: 
				// predany parametr je typu INT
				nd->type = AST_INT;
				nd->dataType = DT_INT;
				nd->data.integer = (*cur)->data.integer;
							
				break;
			case T_REAL: 
				// predany parametr typu real
				nd->type = AST_REAL;
				nd->dataType = DT_REAL;
				nd->data.real = (*cur)->data.real;
				
				break;
			case T_KW_TRUE: 
				// predany parametr typu bool
				nd->type = AST_BOOL;
				nd->dataType = DT_BOOL;
				nd->data.boolean = true;
				break;
			case T_KW_FALSE: 
				// predany parametr typu bool
				nd->type = AST_BOOL;
				nd->dataType = DT_BOOL;
				nd->data.boolean = false;
				break;
			case T_STR:
				// predany parametry typu string
				nd->type = AST_STR;
				nd->dataType = DT_STR;
				copyString((*cur)->data.str, &(nd->data.str));		
				break;
			default:
				E("callPars: Syntax error - invalid parameter type");
				printTokenType((*cur));
				exit(synt);
		}
		
		// nacteni oddelovace
		(*cur) = getToc();
		if((*cur)->type == T_COM){
			// nacetl carku - v poradku pokud nenasleduje T_RPAR
			(*cur) = getToc();
			if((*cur)->type == T_RPAR){
				E("callPars: Syntax error - comma before right parenthesis");
				//printTokenType((*cur));
				exit(synt);
			}
		}
		
		// vytvoreni polozky a vlozeni polozky do node
		D("Pushing into call queue");
		printAst(nd);
		queuePush(pars, nd);
	}
	
	D("Expected data - no received");
	return pars;
}
/**
 * Porovnava data z definovanch parametru s predanymi
 * --------------------
 * callParams: parametry z volani
 * funcParams: parametry z definice
 */
void controlCallParams(struct queue* callParams, struct queue* funcParams){
	W("controlCallParams");
	if(callParams == NULL || funcParams == NULL){
		E("control: NULL parameters queue");
		exit(intern);
	}
	
	if(callParams->length != funcParams->length){
		E("Semantic error - wrong parameter count");
		exit(sem_komp);
	}
	
	struct queueItem* qi1,* qi2;
	qi1 = callParams->start;
	qi2 = funcParams->start;

	struct astNode* qi1ast;
	struct astNode* qi2ast;
	while(qi1 && qi2){
		// dokud sedi parametry podle deklarace
		qi1ast = (struct astNode*)qi1->value;
		qi2ast = (struct astNode*)qi2->value;
		
		// porovnani typu parametru
		if(qi1ast->dataType != qi2ast->dataType){
			E("Semantic error - not same types of parameter");
			datatypes(qi1ast->dataType, qi2ast->dataType);
			exit(sem_komp);
		}
				
		// nastaveni na dalsi parametr
		qi1 = qi1->next;
		qi2 = qi2->next;
		
		// podminky na pocet parametru
		if(qi2 != NULL){
			if(qi1 != NULL){
				// ok
				continue;
			}
			else {
				// mene parametru
				E("Semantic error - few parameters for func call");
				exit(sem_komp);
			}
		}
		else {
			if(qi1 != NULL){
			 	// vic parametru nez definovanych
			 	E("Semantic error - too many parameters");
			 	exit(sem_komp);
			}
		}
	}
}
/**
 * Kontroluje parametry dopredne deklarace oproti definici
 * --------------------
 * defPars: Parametry z definice
 * decPars: Parametry z dopredne deklarace
 */
void controlDefinitionParams(struct queue* defPars, struct queue* decPars){
	W("controlDefinitionParams");
	if(defPars == NULL || decPars == NULL){
		E("control: NULL parameters queue");
		exit(intern);
	}
	
	struct queueItem* defs = defPars->start;
	struct queueItem* decs = decPars->start;

	struct astNode* def;
	struct astNode* dec;
	struct String* defname;
	struct String* decname;
	
	while(decs != NULL){
		def = (struct astNode*)defs->value;
		dec = (struct astNode*)decs->value;
	
		defname = def->data.str;
		decname = dec->data.str;
				
		// porovnani nazvu promennych
		if(compareStrings(defname, decname) != 0){
			E("Semantic error - expected same parameter name");
			exit(sem_komp);
		}
	
		// porovnani datovych typu
		if(def->dataType != dec->dataType){
			// nesedi datove typy
			E("Semantic error - expected same parameter type");
			exit(sem_komp);
		}
	
		defs = defs->next;
		decs = decs->next;
		if(decs != NULL){
			if(defs == NULL){
				// malo parametru predanych
				E("Semantic error - few parameters");
				exit(sem_komp);
			}
			else {
				continue;
			}
		}
		else {
			if(defs != NULL){
				E("Semantic error - too many parameters");
				exit(sem_komp);
			}
		}
	}	
}

/**
 * Parsuje volani funkce, kde uz dostane predany T_ID od exprParseru
 * --------------------
 * id: Token ID se jmenem funkce
 */
struct astNode* parseFuncCall(struct toc** id){
	W("parseFuncCall");
	// v cyklu nasel T_ID a nasledne T_LPAR --> pravdepodobne volani funkce
	
	struct astNode* node = makeNewAST();	
	node->type = AST_CALL;
	
	// skopirovani jmena promenne
	struct String* name = makeNewString();
	copyString((*id)->data.str, &name);
	node->other = name;
	
	// hledani funkce, jestli vubec byla definovana
	struct symbolTableNode* func = search(&global.funcTable, name);
	if(!func){
		E("call: Semantic error - nondefined function");
		exit(sem_prog);	
	}
	
	// z definice vytahnout navratovy typ
	node->dataType = func->dataType;	
	
	// ulozeni parametru a promennych
	node->right = makeNewAST();
	// parametry volani
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = parseCallParams(id);
	node->right->other = vp;
	
	struct astNode* fnode = (struct astNode*)func->other;
	if(!fnode){
		E("Semantic error - calling undefined function");
		exit(sem_prog);
	}
	struct astNode* vpnode = fnode->right;
	struct varspars* vpfunc = (struct varspars*)vpnode->other;

	D("vp call");
	printVarsPars(vp);
	D("vp func");
	printVarsPars(vpfunc);
	controlCallParams(vp->pars, vpfunc->pars);
	node->left = NULL;

	*id = getToc();
	//printTokenType(*id);
	
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
	
	if(condition->dataType != DT_BOOL){
		E("Semantic error - if condition has not BOOL data type");
		exit(sem_komp);
	}
		
	expect((*cur), T_KW_THEN, synt);
		
	// vytvoreni nove polozky
	struct astNode* ifnode = makeNewAST();
	if(!ifnode)
		return NULL;
	// nastaveni znamych informaci
	ifnode->type = AST_IF;
	ifnode->other = condition;
	
	// telo true
	*cur = getToc();
	expect(*cur, T_KW_BEGIN, synt);
	
	ifnode->left = parseBody(cur, true, T_KW_END);
	if(!ifnode->left)
		return NULL;

	// ziskani dalsiho tokenu
	if(expect(*cur, T_KW_END, 0))
		*cur = getToc();

	// pokud else pak pokracovat telem
	if((*cur)->type == T_KW_ELSE){
		*cur = getToc();
		expect(*cur, T_KW_BEGIN, synt);
		
		ifnode->right = parseBody(cur, true, T_KW_END);
		if(!ifnode->right)
			return NULL;

		
		// ziskani dalsiho tokenu
		if(expect(*cur, T_KW_END, 0))
			*cur = getToc();
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
	
	if(condition->dataType != DT_BOOL){
		E("Semantic error - if condition has not BOOL data type");
		exit(sem_komp);
	}
	
	expect((*cur), T_KW_DO, synt);

	struct astNode* node = makeNewAST();				
	// ulozeni podminky
	node->other = condition;
	node->type = AST_WHILE;
	
	// kompletace tela
	*cur = getToc();
	expect(*cur, T_KW_BEGIN, synt);
	node->left = parseBody(cur, true, T_KW_END);
	node->right = NULL;

	// ocekavani endu po BODY
	if(expect(*cur, T_KW_END, 0))
		*cur = getToc();
			
	// navraceni stromu 
	return node;
}

/**
 * Parsuje REPEAT cyklus
 * --------------------
 * cur: Vraci tudy posledni nacteny token
 */
struct astNode* repeatStatement(struct toc** cur){
	/*
	 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 * 	REPEAT-UNTIL PRIMARNE NEPOUZIVA BEGIN-END
	 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 */
	
	struct astNode* rpt = makeNewAST();
	if(!rpt)
		return NULL;
	rpt->type = AST_REPEAT;
	// telo cyklu
	rpt->left = parseBody(cur, false, T_UNTIL);
	if(!rpt->left)
		return NULL;
	
	// nacetl UNTIL
	struct astNode* tmp = parseExpression(cur);	
	if(tmp->dataType != DT_BOOL){
		E("Semantic error - if condition has not BOOL data type");
		exit(sem_komp);
	}
	rpt->other = tmp;
	// tezko rict nestli bude cist nebo ne
	// expresion se zastavi na tom tokenu ktery nezna	
	
	return rpt;
}
struct astNode* forStatement(struct toc** cur){
	// nacteny for ocekavat ID > ASGN -> To/Downto -> lteral -> body
	struct astNode* forNode = makeNewAST();
	forNode->type = AST_FOR;		
	
	// ocekavat ID, jenom
	struct astNode* forCond = makeNewAST();
	
	*cur = getToc();
	expect(*cur, T_ID, synt);			
	struct symbolTableNode* var = searchOnTop((*cur)->data.str);

	// spatny datovy typ
	if(var->dataType != DT_INT){
		E("for: Semantic error - for variable has not INT data type");
		exit(sem_komp);
	}
	
	// ulozeni idcka doleva
	forCond->left = makeNewAST();
	forCond->left->type = AST_ID;		
	copyString((*cur)->data.str, &(forCond->left->data.str));
	forCond->left->dataType = var->dataType;
	
	// dalsi token je prirazeni
	*cur = getToc();
	expect(*cur, T_ASGN, synt);
	forCond->type = AST_ASGN;
	
	// ocekavat pravou stranu prirazeni
	*cur = getToc();
	expect(*cur, T_INT, synt);

	forCond->right = makeNewAST();
	// nastaveni prave casti foru
	forCond->right->type = AST_INT;
	forCond->right->dataType = DT_INT;
	forCond->right->data.integer = (*cur)->data.integer;
	
	// ocekavat TO/DOWNTO
	*cur = getToc();
	if((*cur)->type == T_KW_TO){
		// nacteny TO keyword
		forNode->right = makeNewAST();
		forNode->right->type = AST_FOR_TO; 
	}
	else if((*cur)->type == T_KW_DOWNTO){
		// nacteny DOWNTO
		forNode->right = makeNewAST();
		forNode->right->type = AST_FOR_DOWNTO;
	}
	else {
		E("Syntax error - expected TO or DOWNTO keyword");
		printTokenType(*cur);
		exit(synt);
	}
	// ulozeni doleva prirazeni, doprava pujde koncova hodnota
	forNode->right->left = forCond;
	
	// nacitat literal
	*cur = getToc();
	forNode->right->right = makeNewAST();
	// koncovy clen
	if((*cur)->type == T_INT){
			forNode->right->right->type = AST_INT;
			forNode->right->right->dataType = DT_INT;
			forNode->right->right->data.integer = (*cur)->data.integer;
	}
	else {
		E("Syntax error - expected int literal after TO/DOWNTO keyword");
		exit(synt);
	}
	
	// nacteni DO za definici
	*cur = getToc();
	expect(*cur, T_KW_DO, synt);
	
	*cur = getToc();
	expect(*cur, T_KW_BEGIN, synt);
	// ocekavani tela
	forNode->left = parseBody(cur, true, T_KW_END);
	D("TELO FOR");
	printAst(forNode->left);
	expect(*cur, T_KW_END, synt);

#ifdef _DEBUG
	D(".................");
	printAst(forNode);
#endif
	return forNode;
}
struct astNode* getCaseElement(struct toc** cur, int dt){
	struct astNode* nd = makeNewAST();	

	// doprava pujde literal oznacujici polozku	
	nd->right = makeNewAST();
			
	switch((*cur)->type){
		case T_INT:
			if(dt != DT_INT){
				E("Semantic error - type compatibility failed");
				exit(sem_komp);
			}
			nd->right->type = AST_INT;
			nd->right->dataType = dt;
			nd->right->data.integer = (*cur)->data.integer;
			break;
		case T_REAL:
			if(dt != DT_REAL){
				E("Semantic error - type compatibility failed");
				exit(sem_komp);
			}
			nd->right->type = AST_REAL;
			nd->right->dataType = dt;
			nd->right->data.real = (*cur)->data.real;
			break;
		case T_BOOL:
			if(dt != DT_BOOL){
				E("Semantic error - type compatibility failed");
				exit(sem_komp);
			}
			nd->right->type = AST_BOOL;
			nd->right->dataType = dt;
			nd->right->data.boolean = (*cur)->data.boolean;
			break;
		case T_STR: 
			if(dt != DT_STR){
				E("Semantic error - type compatibility failed");
				exit(sem_komp);
			}
			nd->right->type = AST_STR;
			nd->right->dataType = dt;			
			nd->right->data.str = (*cur)->data.str;
			break;
		case T_KW_ELSE:
			nd->right->type = AST_NONE;
			nd->right->dataType = dt;
			break;
		default:
			E("Syntax error - expected literal as case identificator");
			printTokenType(*cur);
			exit(synt);
	}
	
	*cur = getToc();
	expect((*cur), T_COL, synt);
	
	// TODO pracovat jen s radkama nebo celym telem
	*cur = getToc();
	if((*cur)->type == T_KW_BEGIN){
		D("Parse body");
		nd->left = parseBody(cur, false, T_KW_END);
		
		expect((*cur), T_KW_END, synt);
	}
	else {
		D("Parse command");
		nd->left = parseCommand(cur);
	
		expect((*cur), T_SCOL, synt);
	}
	return nd;
}
struct astNode* caseStatement(struct toc** cur){
	// konstrukci bude uvozovat AST_SWITCH, 
	// kde v levem podstromu bude fronta case polozek, 
	// kde v levem poduzlu bude literal, oznacuji­ci­ ke 
	// ktere hodnote je tento prvek prirazen, a v pravem poduzlu bude 
	// telo polozky. V pravem poduzlu hlavn­iho uzlu je AST_ID urcujici­, 
	// podle ceho se porovnavaji. 
	struct astNode* switchNode = makeNewAST();
	switchNode->type = AST_SWITCH;
	
	*cur = getToc();
	expect((*cur), T_ID, synt);
	
	struct symbolTableNode* var = searchOnTop((*cur)->data.str);
		
	// vytvoreni jmena promenne
	copyString((*cur)->data.str, &(switchNode->data.str));
	
	// TODO zkontrolovat toto
	// vytvoreni uzlu pro promennou
	switchNode->right = makeNewAST();
	switch((*cur)->type){
		case T_INT:
			switchNode->right->type = AST_INT;
			switchNode->right->dataType = DT_INT;
			break;
		case T_REAL:
			switchNode->right->type = AST_REAL;
			switchNode->right->dataType = DT_REAL;
			break;
		case T_BOOL:
			switchNode->right->type = AST_BOOL;
			switchNode->right->dataType = DT_BOOL;
			break;
		case T_STR:
			switchNode->right->type = AST_STR;
			switchNode->right->dataType = DT_STR;
			break;
	}
	
	// ocekavani OF
	*cur = getToc();
	expect((*cur), T_OF, synt);
	
	// naplneni fronty prikazy
	struct queue* cases = makeNewQueue();
	
	*cur = getToc();
	while((*cur)->type != T_KW_END){
		struct astNode* node = getCaseElement(cur, var->dataType);
		
		queuePush(cases, node);
		
		// nacteni dalsiho tokenu
		*cur = getToc();
	}
	
	switchNode->left = makeNewAST();
	switchNode->left->other = cases;
	
	return switchNode;
}

// INLINE FUNKCE
struct astNode* writeStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_WRITE;

	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
			
	*cur = getToc();
	expect((*cur), T_LPAR, synt);
	
	*cur = getToc();
	if((*cur)->type == T_RPAR){
		E("Syntax error - write must have at least one parameter");
		exit(synt);
	}
	
	// nacitat parametry
	while((*cur)->type != T_RPAR){
		// ocekavat cokoliv
		struct astNode* nd = makeNewAST();
		struct symbolTableNode* var;
		
		switch((*cur)->type){
			case T_ID:
				var = searchOnTop((*cur)->data.str);
				// vytvorit novy uzel
				nd->type = AST_ID;				
				// nazev promenne
				copyString((*cur)->data.str, &(nd->data.str));
				// datovy typ promenne
				nd->dataType = var->dataType;
				
				break;
			case T_INT: 
				// novy uzel
				nd->type = AST_INT;
				nd->dataType = DT_INT;
				nd->data.integer = (*cur)->data.integer;
				break;
			case T_REAL: 
				// novy uzel
				nd->type = AST_REAL;
				nd->dataType = DT_REAL;
				nd->data.real = (*cur)->data.real;						
				break;
			case T_STR: 
				// novy uzel
				nd->type = AST_STR;
				nd->dataType = DT_STR;
				nd->data.str = (*cur)->data.str;
				
				break;
			case T_BOOL: 						
				// novy uzel
				nd->type = AST_BOOL;
				nd->dataType = DT_BOOL;
				nd->data.boolean = (*cur)->data.boolean;
				break;		
			default:
				E("Semantic error - unsupported type of parameter");
				//printTokenType(*cur);
				exit(sem_komp);
		}
				
		D("WRITE PARAMETER");
		printAst(nd);
		D("/WRITE PARAMETER");
				
		// push do fronty
		queuePush(vp->pars, nd);						
				
		// dalsi token + dopredu
		*cur = getToc();
		// oddelene carkou -> kdyz za carkou bude RPAR chyba				
		if((*cur)->type == T_COM){
			// nactena carka
			*cur = getToc();
			if((*cur)->type == T_RPAR){
				// za carkou byla prava zavorka
				E("Syntax error - after comma cannot be right parenthesis in params list");
				exit(synt);
			}
		}
	}

	// kontrola na konec parametru		
	expect((*cur), T_RPAR, synt);

	// posledni token	
	*cur = getToc();
			
	node->right = makeNewAST();
	node->right->other = vp;
	return node;			
}
struct astNode* readlnStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_READLN;
		
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	// pokud nenasleduje ID -> chyba
	*cur = getToc();
	expect((*cur), T_ID, synt);
	
	// jeden parametry - ID
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	struct symbolTableNode* var = searchOnTop((*cur)->data.str);
	if(var->dataType == DT_BOOL){
		E("Semantic error - readln parameter cannot be BOOL");
		exit(sem_komp);
	}

	struct astNode* nd = makeNewAST();
	nd->type = AST_ID;
	nd->dataType = var->dataType;
	copyString((*cur)->data.str, &(nd->data.str));
	queuePush(vp->pars, nd);
		
	node->right = makeNewAST();
	node->right->other = vp;

	// kontrola na konec
	*cur = getToc();
	expect((*cur), T_RPAR, synt);

	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->left = NULL;
	return node;
}
struct astNode* findStatement(struct toc** cur){
	struct astNode* find = makeNewAST();
 	find->type = AST_FIND;
	find->dataType = DT_INT;

	// nacist zacatek parametru	
	*cur = getToc();
	expect((*cur), T_LPAR, synt);
	
	find->right = makeNewAST();
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
			
	for(int i = 0; i < 2; i++){
		// iterace, ocekavam dva stejny parametry		
		*cur = getToc();
		if((*cur)->type == T_ID){
			// nacitani z promenne -> tabulka symbolu
			struct symbolTableNode* var = searchOnTop((*cur)->data.str);
		
			if(var->dataType != DT_STR){
				E("Semantic error - INLINE find expected STRING as parameter");
				exit(sem_komp);
			}
		
			struct astNode* first = makeNewAST();
			first->type = AST_ID;
			first->dataType = DT_STR;
			copyString((*cur)->data.str, &(first->data.str));

			// novy parametr
			queuePush(vp->pars, first);
		}
		else if((*cur)->type == T_STR){
			// nacitani literalu
			struct astNode* first = makeNewAST();
			first->type = AST_STR;
			first->dataType = DT_STR;
			first->data.str = makeNewString();
			copyString((*cur)->data.str, &(first->data.str));

			// novy parametr
			queuePush(vp->pars, first);
		}
		else {
			// vypis kde co ocekaval
			if(i == 0){
				E("Syntax error - INLINE find expected string as first parameter data type");
			}
			else {
				E("Syntax error - INLINE find expected string as second parameter data type");
			}
			//printTokenType(*cur);
			exit(synt);
		}
			
		// oddelovac nebo prava zavorka
		*cur = getToc();
		if(i == 0){
			// ocekavam carku
			expect((*cur), T_COM, synt);
		}
		else {
			// ocekavam pravou zavorku
			expect((*cur), T_RPAR, synt);
		}
	}
		
	// dalsi token
	*cur = getToc();			
	
	find->right = makeNewAST();
	find->right->other = vp;
	return find;
}
struct astNode* sortStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_SORT;
	node->dataType = DT_STR;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	// nacist jeden string parametr
	*cur = getToc();
	expect((*cur), T_LPAR, synt);
		
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct symbolTableNode* var = searchOnTop((*cur)->data.str);

		if(var->dataType != DT_STR){
			E("Semantic error - expected string as parameter data type");
			exit(sem_komp);
		}
		
		// uzel pro promennou
		struct astNode* nd = makeNewAST();
		nd->type = AST_ID;
		nd->dataType = DT_STR;
	
		// jmeno promenne
		copyString(var->name, &(nd->data.str));

		// ulozit do seznamu
		queuePush(vp->pars, nd);
	}
	else if((*cur)->type == T_STR){
		// novy uzel pro literal
		struct astNode* nd = makeNewAST();
		nd->type = AST_STR;
		nd->dataType = DT_STR;
		copyString((*cur)->data.str, &(nd->data.str));		

		// pushnuti do fronty
		queuePush(vp->pars, nd);
	}
	else {
		E("Semantic error - expected string as parameter data type");
		exit(sem_komp);
	}

	// konec parametru
	*cur = getToc();
	expect((*cur), T_RPAR, synt);
	
	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->right = makeNewAST();
	printVarsPars(vp);
	node->right->other = vp;
	
	return node;
}
struct astNode* lengthStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_LENGTH;
	node->dataType = DT_INT;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	// nacist jeden string parametr
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	// parametr
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct symbolTableNode* var = searchOnTop((*cur)->data.str);

		if(var->dataType != DT_STR){
			E("Semantic error - expected string as parameter data type");
			exit(sem_komp);
		}
		
		// uzel pro promennou
		struct astNode* nd = makeNewAST();
		nd->type = AST_ID;
		nd->dataType = DT_STR;	
		// jmeno promenne
		copyString(var->name, &(nd->data.str));
	
		queuePush(vp->pars, nd);
	}
	else if((*cur)->type == T_STR){
		// novy uzel pro literal
		struct astNode* nd = makeNewAST();
		nd->type = AST_STR;
		nd->dataType = DT_STR;
		copyString((*cur)->data.str, &(nd->data.str));
		
		// pushnuti do fronty
		queuePush(vp->pars, nd);
	}
	else {
		E("Semantic error - expected string as parameter data type");
		exit(sem_komp);
	}
	
	*cur = getToc();
	expect((*cur), T_RPAR, synt);
	
	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->right = makeNewAST();
	node->right->other = vp;
	
	return node;
}
struct astNode* copyStatement(struct toc** cur){
	struct astNode* copy = makeNewAST();
	copy->type = AST_COPY;
	copy->dataType = DT_STR;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	*cur = getToc();
	if((*cur)->type == T_ID){
		// najit v tabulce symbolu
		struct symbolTableNode* var = searchOnTop((*cur)->data.str);
		
		// novy uzel
		struct astNode* nid = makeNewAST();
		nid->type = AST_ID;
		nid->dataType = var->dataType;		
		// kopie jmena
		copyString((*cur)->data.str, &(nid->data.str));
		
		// push prvniho parametru
		queuePush(vp->pars, nid);				
	}
	else if((*cur)->type == T_STR){
		// predelat do nodu		
		struct astNode* nstr = makeNewAST();
		nstr->type = AST_STR;
		nstr->dataType = DT_STR;
		copyString((*cur)->data.str, &(nstr->data.str));

		// push
		queuePush(vp->pars, nstr);				
	}
	else {
		E("Semantic error - expected STRING data type of parameter");
		exit(sem_komp);
	}
	
	*cur = getToc();
	expect((*cur), T_COM, synt);
	
	// ocekavat 2x int
	for(int i = 0; i < 2; i++){
		*cur = getToc();

		if((*cur)->type == T_ID){
			struct symbolTableNode* var = searchOnTop((*cur)->data.str);
			
			// kontrola typu - oba musis byt INT
			if(var->dataType != DT_INT){
				E("Semantic error - variable of parameter has wrong type");
				exit(sem_komp);
			}
					
			// novy uzel
			struct astNode* nid = makeNewAST();
			nid->type = AST_ID;
			nid->dataType = var->dataType;			
			// kopie jmena
			copyString((*cur)->data.str, &(nid->data.str));
			
			// push prvniho parametru
			queuePush(vp->pars, nid);		
		}
		else if((*cur)->type == T_INT){
			// novy uzel		
			struct astNode* nint = makeNewAST();
			nint->type = AST_INT;
			nint->dataType = DT_INT;
			nint->data.integer = (*cur)->data.integer;
			
			// pushnuti do fronty
			queuePush(vp->pars, nint);					
		}
		else {
			E("Semantic error - last two parameters must be integers");
			exit(sem_komp);
		}
		
		
		*cur = getToc();
		if(i == 0)
			expect((*cur), T_COM, synt);
		else
			expect((*cur), T_RPAR, synt);
	}
	
	expect((*cur), T_RPAR, synt);

	// nacteni posledniho tokenu
	*cur = getToc();
	
	copy->right = makeNewAST();
	copy->right->other = vp;
	return copy;	
}

struct astNode* arrayIndexStatement(struct toc** cur){
	struct astNode* arrid = makeNewAST();
	arrid->type = AST_ARR;
	struct String* name = NULL;
	copyString((*cur)->data.str, &name);
	arrid->other = name;

	// integer jako index			
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct symbolTableNode* var = searchOnTop((*cur)->data.str);
		
		// kontrola na typ
		if(var->dataType != DT_INT){
			E("Semantic error - variable of parameter has wrong type");
			exit(sem_komp);
		}

		struct astNode* nid = makeNewAST();
		nid->type = AST_ID;
		nid->dataType = var->dataType;
		copyString((*cur)->data.str, &(nid->data.str));

		// ulozeni doleva
		arrid->left = nid;
		arrid->right = NULL;
	}
	else if((*cur)->type == T_INT){
		// index do datove struktury
		arrid->data.integer = (*cur)->data.integer;
		arrid->left = NULL;
		arrid->right = NULL;
	}
	else {
		E("Semantic error - unsupported data type of array index");
		exit(sem_komp);
	}

	// nacitani prave zavorky
	*cur = getToc();
	expect((*cur), T_RBRC, synt);

	// posledni token
	*cur = getToc();
	return arrid;
} 

struct astNode* assignStatement(struct toc** cur){
	// levy uzel je T_ID -- z parse command
	// pravy uzel je expression
	struct astNode* left = makeNewAST();				
	left->type = AST_ID;
	copyString((*cur)->data.str, &(left->data.str));
		
	// skopirovani informaci z tabulky symbolu
	struct symbolTableNode* nd = searchOnTop(left->data.str);				
	left->dataType = nd->dataType;
	
	struct astNode* right = parseExpression(cur);		
	
	if(left->dataType != right->dataType){
		datatypes(left->dataType, right->dataType);
		E("cmd: Semantic error - L-value has not same value as R-value");
		exit(sem_komp);
	}
								
	// vytvorit uzel prirazeni a vratit jej
	struct astNode* asgn = makeNewAST();
	asgn->type = AST_ASGN;
	asgn->left = left;
	asgn->right = right;
	asgn->other = NULL;
	return asgn;
}
/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * ------------------------------------------------------------------------------------
 */
struct astNode* parseCommand(struct toc** cur){
	W("parseCommand");

	
	struct astNode* node = NULL;
	struct toc* next;
	
#ifdef _DEBUG
	printTokenType(*cur);
#endif

	// prvni token je nacteny
	switch((*cur)->type){
		case T_KW_BEGIN:
			// dalsi blok kodu
			node = parseBody(cur, true, T_KW_END);
			// nacist dalsi token
			*cur = getToc();
			return node;
		case T_KW_IF:
			D("IF command");		
			node = ifStatement(cur);
			if(!node)
				return NULL;

			return node;
		case T_KW_WHILE:
			D("WHILE command");
			node = whileStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_RPT:
			D("REPEAT command");
			node = repeatStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_CASE:
			D("CASE cmd");
			node = caseStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_FOR:
			/* for(asgn, int, int) */
			D("FOR cmd");			
			node = forStatement(cur);
			if(!node)
				return NULL;
			return node;	
		case T_KW_WRT:
			/* write(params) */
			D("WRITE");
			node = writeStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_READLN:
			/* readln(id) */
			D("READLN");
			node = readlnStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_ID:
			D("ASSIGN");
			// podle next tokenu se rozlisi o co jde
			next = getToc();
			expect(next, T_ASGN, synt);
			// v cur je ID
			node = assignStatement(cur);
			if(!node){
				D("ASGN node is null");
				return NULL;
			}
			printTokenType(*cur);
			return node;
		default:
			E("Syntax error - unexpected token type");
			printTokenType(*cur);
			exit(synt);
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

	bool readNew = true;
	//struct astNode *cmd = NULL;
	

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
			*cur = id;
			// <id>(.....	
			D("expr: FUNC CALL");
			struct astNode* nd = parseFuncCall(cur);
			if(!nd) return NULL;

			// ulozit na zasobnik funkci
			stackPush(aststack, nd);
		}
		else if((*cur)->type == T_LBRC){
			// indexing...
		
		}		
		else {
			D("expr: VARIABLE");
			// v pripade, ze po ID nebyla zavorka
			// pushnout na stack, jako promennou
			struct astNode* node = makeNewAST();
			node->type = AST_ID;
			// kopie jmena
			copyString(id->data.str, &(node->data.str));
				
			struct symbolTableNode* nd = searchOnTop(node->data.str);
						
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
	// 		pokud je na vrcholu parseExleva zavorka
	// 		pokud je na vrcholu operator s nizsi prioritou
	// pokud je na vrcholu operator s vyssi prioritou -> 
	// 		presunout na vystup a opakovat 3. krok
	// 4. prava zavorka -> odebirat ze zasobniku a davat na vystup
	// 		dokud nenarazi na levou zavorku
	// 5. v pripade jineho tokenu -> vyprazdnovat zasobnik na vystup
	// 		tokeny ;, THEN, DO by mely vesmes ukoncovat vyrazy	
	while((*cur) != NULL){
		D("expr: new token");
#ifdef _DEBUG
		fprintf(stderr, "readNew type = %d \n", readNew);
		printTokenType(*cur);
#endif		
		struct toc* t;
		struct toc* top;
		struct toc* now;
		struct astNode* node;
		
		switch((*cur)->type){
			case T_KW_LENGTH:
				W("builtin -- length");
				node = lengthStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_SORT:
				W("builtin -- sort");
				node = sortStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_FIND:
				W("builtin -- find");
				node = findStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_COPY:
				W("builtin -- copy");
				node = copyStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			// leva zavorka
			case T_LPAR: 
				W("T_LPAR comes");
			
				stackPush(stack, (*cur));
				break;
			// prava zavorka
			case T_RPAR:
				W("T_RPAR comes");
			
				t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					E("expression: Syntax error - no Left parenthesis before Right parenthesis");
					exit(synt);
				}
				
				while(!stackEmpty(stack) && (t->type != T_LPAR)){
					//printTokenType(t);
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					makeAstFromToken(t, &aststack);
					
					t = (struct toc*)stackPop(stack);				
					// popnul vsechno do leve zavorky
				}				
				break;
			// operandy
			case T_ID:
			case T_INT:
			case T_REAL:
			case T_STR:
			case T_BOOL: 
			case T_KW_TRUE:
			case T_KW_FALSE:
				W("Literal comes");
				//printTokenType(*cur);
				
				node = makeNewAST();
				
				if((*cur)->type == T_ID){
					// nacteni dalsiho tokenu
					now = getToc();
					if(now->type == T_LPAR){
						// volani funkce
						node = parseFuncCall(cur);	
					}
					else {
						// promenna			
						node->type = AST_ID;
						// kopie jmena
						copyString((*cur)->data.str, &(node->data.str));
					
						// ziskani tabulky symbolu
						struct symbolTableNode* nd = searchOnTop(node->data.str);
						D("expr: comparison between symtable node and ast node");
						datatypes(nd->dataType, node->dataType);
					
						node->dataType = nd->dataType;					
						D("expr: making ID");
						*cur = now;
					}
					// nahrat nacteny token
					
					D("READ NEW - cur type");
					//printTokenType(*cur);now
					readNew = false;
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
					node->data.str = (*cur)->data.str;
						
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
				stackPush(aststack, node);
				break;
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
			case T_NOT:
				W("Operator comes");
				//printTokenType(*cur);
				// 3. operator:
				// 		pokud je zasobnik prazdny
				// 		pokud je na vrcholu leva zavorka
				// 		pokud je na vrcholu operator s nizsi prioritou
				// pokud je na vrcholu operator s vyssi prioritou -> 
				// 		presunout na vystup a opakovat 3. krok
				if(stackEmpty(stack)){
					// v pripade, ze je to prvni zaznam do stacku, tak ho tam proste vrazit
					stackPush(stack, (*cur));
				}
				else {	
					top = (struct toc*)stackTop(stack);
					/*
					if(top->type == T_LPAR){
						E("Syntax error - wrong expression syntax");
						exit(synt);
					}
					*/
					
					// neni to prvni zapis, nutno zkontrolovat priority
					while(!stackEmpty(stack) && (getPriority(top) >= getPriority(*cur))){
						// odebrat vrchol ze zasobniku
						top = (struct toc*)stackPop(stack);
						
						// vytvori ASTnode z tokenu
						makeAstFromToken(top, &aststack);
						
						// na promennou za vrchol ulozi top prvek
						top = (struct toc*)stackTop(stack);
					}		
					
					// ulozit aktualni prvek na vrchol zasobniku
					stackPush(stack, (*cur));
				}	
				break;
			case T_KW_END:
			case T_KW_ENDDOT:
			case T_SCOL:
			case T_KW_DO:
			case T_KW_THEN:
				// vyprazdnit vsechny operatory v zasobniku a postavit nad nimi strom
				D("expr: Making node into");
				//printTokenType(*cur);
				
				while(!stackEmpty(stack)){
					D("expr: STACK STATE START =======");
				
					now = (struct toc*)stackPop(stack);
					//printTokenType(now);					
					
					makeAstFromToken(now, &aststack);
						
					D("expr: STACK STATE END =========");
					printAstStack(aststack);
				}				
			
				// v pripade, ze je v zasobniku jen jeden prvek
				// vratit ho, je to vysledek SY algoritmu
				if(aststack->Length != 1){
					E("expression: Shunting yard error - wrong expression syntax");
					printAstStack(aststack);
					exit(synt);
				}
			
				// vrati vrchol 
				D("expression: Returning top layer");
				return stackPop(aststack);
			default:
				E("expression: Syntax error - bad token type");			
				//printTokenType(*cur);
				exit(synt);
		}
		
		// get next token
		if(readNew){
			(*cur) = getToc();
			//printTokenType(*cur);
			D("Read new token");
		}
		else{
			readNew = true;
			D("dont read new token");
		}
	}
	E("expression: Syntax error");
	exit(synt);
}

