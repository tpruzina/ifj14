#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Stack.h"
#include "Log.h"
#include "Ast.h"
#include "Scanner.h"
#include "Parser.h"
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
	{"T_RPT",T_KW_RPT},
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
		if(token->type == tokennames[i].type){
			fprintf(stderr, "Token: %s\n", tokennames[i].str);
			
			if(token->type == T_ID){
				fprintf(stderr, "\t%s\n", token->data.str->Value);
			}
		}
	}
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
#ifdef _DEBUG
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
			exit(sem_prog);
		}
	}
	
	
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
			return False;
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
		return False;						
	}
	
	D("Printing ast");
	// ulozeni zpatky na zasobnik
	int ret = stackPush(*aststack, node);
	printAst((struct astNode*)stackTop(*aststack));
	return ret;
}

/**
 * Tiskne obsah zasobniku tabulky symbolu
 * --------------------------------------
 */
void printSymbolTableStack(){
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
	
	global.program = prog;
	return True;
}

int checkFunctionDeclarations(struct symbolTableNode* gft){
	// pokud predana tabulka neni prazdna
	if(gft == NULL){
		return False;
	}
	
	// telo funkce
	struct astNode* telo = (struct astNode*)gft->other;
	if(telo == NULL){
		E("Function without ast node deklaration");
		exit(sem_prog);
	}
	
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
	struct toc* cur = getToc();

	if(cur->type != T_KW_PROGRAM){
		E("Syntax error - expected PROGRAM keyword");
		global.errno = synt;
		return NULL;
	}
	
	
	struct astNode* program = makeNewAST();
	program->type = AST_START;


	// nalezen start
	cur = getToc();
	if(cur->type != T_ID){
		// chyba, melo nasledovat oznaceni
		W("Warning - expected name of program");
	}
	else {	
		// kopirovani jmena programu
		copyString(cur->data.str, &(program->data.str));
	
		// v pripade, ze bylo nactene jmeno, nacist dalsi
		cur = getToc();
	}
	
	if(cur->type != T_SCOL){
		E("Syntax error - expected SEMICOLON");
		exit(synt);
	}
					
	// ok -> nasleduje telo programu nebo definice promennych nebo definice 
	// funkci/dopredne definice funkci			
	// pokud bude nasledujici token BEGIN pak ctu telo
	// pokud bude nasledujici VAR pak jsou to lokalni 
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = parseVars(&cur);
	// lokalni promenne
	program->other = vp;
	printVarsPars(vp);
	

	// prochazet definice funkci a vkladat je s telem do tabulky symbolu
	while(cur->type == T_KW_FUNC){
		D("Another function");
		struct astNode* func = parseFunction();				
		if(!func)
			return NULL;
		printAst(func);
		
		// muze nacitat 
		cur = getToc();		
		if(cur->type == T_SCOL)
			cur = getToc();
				
		//printTokenType(cur);		
	}
	//printTokenType(cur);
	
	//printTokenType(cur);
	
	// nasleduje telo programu
	struct astNode* body = parseBody(&cur);		
	if(!body)
		return NULL;
	program->left = body;
	
	D("program: after body");
	//printTokenType(cur);
	
	cur = getToc();
	if(cur->type != T_DOT){
		E("program: Syntax error - expected END.");
		printTokenType(cur);
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
	body->type = AST_CMD;
	
	*cur = getToc();
	if((*cur)->type == T_KW_END){
		// prazdne telo 
		D("PRAZDNE TELO");
		return body;	
	}
	
	struct astNode* cmd = parseCommand(cur);
	if((*cur)->type == T_KW_END){
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
		// ocekavat dalsi command
		while(cmd != NULL){
			struct astNode* item = body;
			while(item->left != NULL)
				item = item->left;
			item->left = makeNewAST();
			item->left->type = AST_CMD;
			item->left->left = NULL;
			item->left->right = cmd;
		
			D("Waiting for end or scol");
			printTokenType(*cur);
			// pokud narazi na END tak ukoncit hledani dalsich 
			if((*cur)->type == T_KW_END)
				break;
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
	return body;
}

/**
 * Parsuje var sekci funkce/programu
 * ---------------------------------
 */
struct queue* parseVars(struct toc** cur){
	W("parseVars");
	
	if((*cur)->type != T_KW_VAR)
		(*cur) = getToc();
		
	if((*cur)->type == T_KW_VAR){
		// zacinat parsovat promenne
		
		struct queue* vars = makeNewQueue();		
		struct symbolTableNode* top = (struct symbolTableNode*)stackPop(global.symTable);
		
		while(1){
			// definice promennych
			struct astNode* var = makeNewAST();
			
			
			(*cur) = getToc();
			if((*cur)->type == T_ID){
				// dostal jsem ID -> ocekavam dvoutecku
			
				// kopie jmena promenne
				struct String* name = makeNewString();
				copyString((*cur)->data.str, &name);
				var->other = name;
			
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
							new = insertValue(&top, name, DT_INT);
								
							if(!new)
								return NULL;
							
							new->dataType = DT_INT;													
							break;
						}
						case T_KW_REAL: {
							var->type = AST_REAL;
							
							new = insertValue(&top, name, DT_REAL);
							if(!new)
								return NULL;
								
							new->dataType = DT_REAL;
							break;
						}
						case T_KW_BOOLEAN: {
							var->type = AST_BOOL;
											
							new = insertValue(&top, name, DT_BOOL);
							if(!new)
								return NULL;
								
							new->dataType = DT_BOOL;
							break;
						}
						case T_KW_STRING: {
							var->type = AST_STR;
														
							new = insertValue(&top, name, DT_STR);
							if(!new)
								return NULL;
								
							new->dataType = DT_STR;
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
															
								// vlozeni nazev pole do tabulky
								new = insertValue(&top, name, DT_ARR);
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
								
				// vlozit prvek
				queuePush(vars, var);
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
				printTokenType(*cur);
				exit(synt);			
			}
		}
		
		printSymbolTable(top, 0);		
		stackPush(global.symTable, top);
				
		// vratit seznam promennych
		return vars;
	}
 
 	D("returning null");
 	//printTokenType(*cur);
	return NULL;
}

/**
 * Parsuje parametry v definici/deklaraci funkce
 * ---------------------------------------------
 */
struct queue* parseParams(){
	W("parseParams");
	
	struct toc* cur = getToc();
	struct queue* params = makeNewQueue();
	if(!params)
		return NULL;
		
		
	if(cur->type != T_LPAR){
		E("params: Syntax error - expected Left parenthesis");
		//printTokenType(cur);
		exit(synt);	
	}
	
	struct symbolTableNode* top = (struct symbolTableNode*)stackPop(global.symTable);
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
				struct String* name = makeNewString();
				copyString(cur->data.str, &name);
				par->other = name;
				
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
						par->type = AST_INT;
						par->dataType = DT_INT;
						break;
					}
					case T_KW_REAL: {
						par->type = AST_REAL;
						par->dataType = DT_REAL;
						break;
					}
					case T_KW_BOOLEAN: {
						par->type = AST_BOOL;
						par->dataType = DT_BOOL;
						break;
					}
					case T_KW_STRING: {
						par->type = AST_STR;
						par->dataType = DT_STR;
						break;
					}
					default:
						E("params: Syntax error - unsupported type of parameter");
						//printTokenType(cur);
						exit(synt);
				}
				
				// ulozit do seznamu a pokracovat
				insertValue(&top, name, par->dataType);	
				queuePush(params, par);
				
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
				//printTokenType(cur);
				exit(synt);	
			}
		
			cur = getToc();
		}
	}
	// pouze v pripade, ze byla zadana prava zavorka vratit seznam
	if(cur->type == T_RPAR){
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
	struct String* name = makeNewString();
	copyString(cur->data.str, &name);
#ifdef _DEBUG
	printString(name);
#endif
	node->other = name;

	// sparsovat parametry
	struct symbolTableNode* newlayer;
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	copyTable(top, &newlayer);
	if(newlayer == NULL){
		E("function: copy failed");
		exit(intern);
	}
	stackPush(global.symTable, newlayer);
	
	// vyhodnoti parametry a vytvori frontu
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->pars = parseParams();
	
	// ziska top
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
	cur = getToc();
	if(cur->type != T_SCOL){
		E("function: Syntax error - expected semicolon after returning type");
		exit(synt);
	}
	
	
	// ulozit do tabulky zastupnou promennou za return
	insertValue(&top, name, node->dataType);

	// prohledani tabulky funkci
	struct symbolTableNode* dekl = (struct symbolTableNode*)search(&(global.funcTable), name);
	
	// DOPREDNA DEKLARACE FUNKCE
	cur = getToc();
	if(cur->type == T_KW_FORW){
		cur = getToc();
		if(cur->type != T_SCOL){
			E("function: Syntax error - expected semicolon after FORWARD keyword");
			exit(synt);
		}
	
		// dopredna deklarace - pokud ji najde ve funkcich je neco spatne
	 	if(!dekl)
		 	dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), name, node->dataType);
		else {
			// nalezl deklarovanou/definovanou funkci v tabulce
			E("function: Syntax error - redefinition of forward declaration");
			exit(synt);
		}
			
		// v pripade dopredne deklarace neni nutne pouzivat parametry
		stackPop(global.symTable);
		//toto nie je nikde pouzite????
		//top = (struct symbolTableNode*)stackTop(global.symTable);
						
	 	// nastaveni navratovy typ
	 	dekl->dataType = node->dataType;
	 		 	
	 	node->right = makeNewAST();
	 	node->right->other = vp;
	 	D("Printing ast node");
	 	printAst(node);	 	
	 		 	
	 	dekl->other = node;
	 	D("FORWARD declaration");
	 	
	 	printSymbolTable(dekl,0);
	 	return node;
	}
	
	D("FUNC DEF");
	
	// zacatek definice
	if(cur->type == T_KW_VAR || cur->type == T_KW_BEGIN){
		if(!(dekl)) {
			// nebyla nalezena - vlozit novy uzel
			dekl = (struct symbolTableNode*)insertValue(&(global.funcTable), name, node->dataType);
			if(!dekl){
				E("function: insertion failed");
				exit(intern);
			}
		}
		
		// v pripade definovani deklarovane funkce
		if(dekl->other != NULL) {
			D("Function has been declared");
			struct astNode* telo = (struct astNode*)(dekl->other);
			struct astNode* vpnode = telo->right;
			if(!vpnode){
				E("Right varspars subnode is empty");
				exit(intern);
			}
			struct varspars* deklpars = (struct varspars*)vpnode->other;
			controlDefinitionParams(vp->pars, deklpars->pars);
	 		D("Control def params DONE");
			if(telo->left != NULL){
				// telo uz bylo jednou definovane!!
				E("function: Syntax error - redefinition of function");
				exit(sem_prog);
			}
		}
	
		if(cur->type == T_KW_VAR){
			// ocekavat var-def --> other
			vp->vars = parseVars(&cur);
			D("parseVars end");
			// po definici promennych MUSI nasledovat telo, tedy begin
			if(cur->type != T_KW_BEGIN){
				E("function: Syntax error - expected BEGIN keyword");
				//printTokenType(cur);
				exit(synt);
			}
		}
		node->right = makeNewAST();
		node->right->other = vp;
		printVarsPars(vp);
	
		if(!dekl->other){
			dekl->other = node;
		}
		
		// ocekavat telo
		D("Body expectation");
		node->left = parseBody(&cur);	
		
		cur = getToc();
		if(cur->type != T_SCOL){
			E("function: Syntax error - expected semicolon after function definition");
			//printTokenType(cur);
			exit(synt);
		}
		
		// odstranit vrchol s parametry a return promennou
		stackPop(global.symTable);			
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
		struct astNode* nd;
		
		switch((*cur)->type){
			case T_ID: {
				// predana promenna
				
				struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
				struct symbolTableNode* id = search(&top, (*cur)->data.str);
				if(!id){
					E("callPars: Undefined variable");
					exit(sem_prog);
				}
								
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_ID;
				copyString((*cur)->data.str, &(nd->data.str));
				nd->dataType = id->dataType;
				datatypes(nd->dataType, id->dataType);
				
				break;
			}
			case T_INT: {
				// predany parametr je typu INT
				
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_INT;
				nd->dataType = DT_INT;
				nd->data.integer = (*cur)->data.integer;
							
				break;
			}
			case T_REAL: {
				// predany parametr typu real
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_REAL;
				nd->dataType = DT_REAL;
				nd->data.real = (*cur)->data.real;
				
				break;
			}
			case T_BOOL: {
				// predany parametr typu bool
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_BOOL;
				nd->dataType = DT_BOOL;
				nd->data.boolean = (*cur)->data.boolean;
				
				break;
			}
			case T_STR: {
				// predany parametry typu string
				if(!(nd = makeNewAST()))
					return NULL;
				
				nd->type = AST_STR;
				nd->dataType = DT_STR;
				copyString((*cur)->data.str, &(nd->data.str));		
				break;
			}
			default:
				E("callPars: Syntax error - invalid parameter type");
				//printTokenType((*cur));
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
	
	struct queueItem* qi1,* qi2;
	qi1 = callParams->start;
	qi2 = funcParams->start;
	
	while(qi2 != NULL){
		// dokud sedi parametry podle deklarace
		struct astNode* qi1ast = (struct astNode*)qi1->value;
		struct astNode* qi2ast = (struct astNode*)qi2->value;
		
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

	while(decs != NULL){
		struct astNode* def = (struct astNode*)defs->value;
		struct astNode* dec = (struct astNode*)decs->value;
	
		struct String* defname = (struct String*)def->other;
		struct String* decname = (struct String*)dec->other;
				
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
	vp->vars = NULL;
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
struct astNode* forStatement(struct toc** cur){
	// nacteny for ocekavat ID > ASGN -> To/Downto -> lteral -> body
	struct astNode* forNode = makeNewAST();
	forNode->type = AST_FOR;	
	
	
	// ocekavat ID, jenom
	struct astNode* forCond = makeNewAST();
	*cur = getToc();
	if((*cur)->type == T_ID){
		// je tam ID
		struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
		if(!top){
			W("Symbol table is empty - no local variables");
		}
		
		struct symbolTableNode* var;
		if(!(var = search(&top, (*cur)->data.str))){
			// nenalezena promenna
			E("for: Semantic error - for variable not found in local variables");
			exit(sem_prog);
		}	
		
		// ulozeni idcka doleva
		forCond->left = makeNewAST();
		forCond->left->type = AST_ID;		
		copyString((*cur)->data.str, &(forCond->left->data.str));
		forCond->left->dataType = var->dataType;
	}
	
	// dalsi token je prirazeni
	*cur = getToc();
	if((*cur)->type == T_ASGN){
		// nasledovalo prirazeni
		forCond->type = AST_ASGN;		
	}
	
	// ocekavat pravou stranu prirazeni
	*cur = getToc();
	struct astNode* lit = makeNewAST();
	D("lit twice alloced");
	switch((*cur)->type){
		case T_INT:
			lit->type = AST_INT;
			lit->dataType = DT_INT;
			lit->data.integer = (*cur)->data.integer;
			break;	
		case T_REAL:
			lit->type = AST_REAL;
			lit->dataType = DT_REAL;
			lit->data.real = (*cur)->data.real;
			break;
		case T_BOOL:
			lit->type = AST_BOOL;
			lit->dataType = DT_BOOL;
			lit->data.boolean = (*cur)->data.boolean;
			break;
		case T_STR: 
			lit->type = AST_STR;
			lit->dataType = DT_STR;
			lit->data.str = (*cur)->data.str;
			break;
		default:
			E("Syntax error - expected literal in right side of assign part of for");
			exit(synt);
	}
	forCond->right = lit;
	
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
	lit = makeNewAST();
	switch((*cur)->type){
		case T_INT:
			lit->type = AST_INT;
			lit->dataType = DT_INT;
			lit->data.integer = (*cur)->data.integer;
			break;	
		case T_REAL:
			lit->type = AST_REAL;
			lit->dataType = DT_REAL;
			lit->data.real = (*cur)->data.real;
			break;
		case T_BOOL:
			lit->type = AST_BOOL;
			lit->dataType = DT_BOOL;
			lit->data.boolean = (*cur)->data.boolean;
			break;
		case T_STR: 
			lit->type = AST_STR;
			lit->dataType = DT_STR;
			lit->data.str = (*cur)->data.str;
			break;
		default:
			E("Syntax error - expected literal after TO/DOWNTO keyword");
			exit(synt);
	}
	// ulozeni koncove hodnoty do praveho uzlu 
	forNode->right->right = lit;
	
	// nacteni DO za definici
	*cur = getToc();
	if((*cur)->type != T_KW_DO){
		E("Syntax error - expected DO keyword afted definition of FOR cycle");
		exit(synt);
	}
	
	// ocekavani tela
	forNode->left = parseBody(cur);
	
	if((*cur)->type != T_KW_END){
		E("Syntax error - expected END keyword at the end of body");
		exit(synt);		
	}
	
	D("Printing astNode of FOR cycle");
	printAst(forNode);
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
	if((*cur)->type != T_COL){
		E("Syntax error - expected COLON as separator in case block");
		//printTokenType(*cur);
		exit(synt);
	}
	
	// TODO pracovat jen s řádkovýma příkazama nebo i s těly
	*cur = getToc();
	if((*cur)->type == T_KW_BEGIN){
		D("Parse body");
		nd->left = parseBody(cur);
		
		if((*cur)->type != T_KW_END){
			E("Syntax error - expected END on body end");
			printTokenType(*cur);
			exit(synt);
		}
		// *cur = getToc();
	}
	else {
		D("Parse command");
		nd->left = parseCommand(cur);
	
		if((*cur)->type != T_SCOL){
			E("Syntax error - expected SEMICOLON at the end of CASE statement");
			printTokenType(*cur);
			exit(synt);	
		}
		
		// *cur = getToc();
	}
	
	D("case element RETURNING");
	printTokenType(*cur);	
	return nd;
}
struct astNode* caseStatement(struct toc** cur){
	// Uzel této konstrukce bude uvozovat AST_SWITCH, 
	// kde v levém podstromu bude fronta case položek, 
	// kde v levém poduzlu bude literál, označující ke 
	// které hodnotě je tento prvek přiřazen, a v pravém poduzlu bude 
	// tělo položky. V pravém poduzlu hlavního uzlu je AST_ID určující, 
	// podle čeho se porovnává. 
	struct astNode* switchNode = makeNewAST();
	switchNode->type = AST_SWITCH;
	
	*cur = getToc();
	if((*cur)->type != T_ID){	
		E("Syntax error - expected ID");
		//printTokenType(*cur);
		exit(synt);
	}
	
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	struct symbolTableNode* var = NULL;
	if(!(var = search(&top, (*cur)->data.str))){
		E("Semantic error - local variable not found");
		exit(sem_prog);
	}
	
	// vytvoreni jmena promenne
	struct String* name = makeNewString();
	copyString((*cur)->data.str, &name);
	
	// TODO zkontrolovat toto
	// vytvoreni uzlu pro promennou
	switchNode->right = makeNewAST();
	switch((*cur)->type){
		case T_INT: {
			switchNode->right->type = AST_INT;
			switchNode->right->dataType = DT_INT;
			break;
		}
		case T_REAL: {
			switchNode->right->type = AST_REAL;
			switchNode->right->dataType = DT_REAL;
			break;
		}
		case T_BOOL: {
			switchNode->right->type = AST_BOOL;
			switchNode->right->dataType = DT_BOOL;
			break;
		}
		case T_STR: {
			switchNode->right->type = AST_STR;
			switchNode->right->dataType = DT_STR;
			break;
		}
	}
	switchNode->right->other = name;
	
	// ocekavani OF
	*cur = getToc();
	if((*cur)->type != T_OF){
		E("Syntax error - expected OF after ID in case definition");
		//printTokenType(*cur);
		exit(synt);
	}
	
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

struct astNode* writeStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_WRITE;

	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = NULL;
	vp->pars = makeNewQueue();
			
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expecting left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	
	// projistotu nacist top		
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	
	*cur = getToc();
	// nacitat parametry
	while((*cur)->type != T_RPAR){
		// ocekavat cokoliv
		struct astNode* nd = makeNewAST();
		struct String* name = NULL;
		switch((*cur)->type){
			case T_ID:
				struct symbolTableNode* var = search(&top, (*cur)->data.str);
				if(!var){
					E("Semantic error - undefined variable");
					exit(sem_prog);
				}
				// vytvorit novy uzel
				nd->type = AST_ID;
				
				// nazev promenne
				name = makeNewString();
				copyString((*cur)->data.str, &name);
				nd->other = name;
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
			case T_RPAR:
				break;		
			default:
				E("Semantic error - unsupported type of parameter");
				//printTokenType(*cur);
				exit(sem_komp);
		}
				
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
			
	if((*cur)->type != T_RPAR){
		E("Syntax error - expecting right parenthesis after call parameters");
		//printTokenType(*cur);
		exit(synt);
	}
		
	*cur = getToc();
			
	node->right = makeNewAST();
	node->right->other = vp;
	return node;			
}

struct astNode* readlnStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_READLN;
		
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expecting left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}

	*cur = getToc();
	if((*cur)->type != T_ID){
		E("Syntax error - expecting identificator as only parameter");
		//printTokenType(*cur);
		exit(synt);
	}
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = NULL;
	vp->pars = makeNewQueue();
	
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	struct symbolTableNode* var = search(&top, (*cur)->data.str);
	if(!var){
		E("Semantic error - variable not found");
		exit(sem_prog);
	}
	struct astNode* nd = makeNewAST();
	nd->type = AST_ID;
	nd->dataType = var->dataType;
	
	struct String* idname = makeNewString();
	copyString((*cur)->data.str, &idname);
	nd->other = idname;
	
	queuePush(vp->pars, nd);
		
	node->right = makeNewAST();
	node->right->other = vp;
	
	*cur = getToc();
	if((*cur)->type != T_RPAR){
		E("Syntax error - expecting right parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	
	*cur = getToc();
	
	node->left = NULL;
	return node;
}

struct astNode* findStatement(struct toc** cur){
	struct astNode* find = makeNewAST();
 	find->type = AST_FIND;
	find->dataType = DT_INT;
			
	struct String* name = makeNewString();
	copyFromArray("find", &name);
	find->other = name;
	
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expecting left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	
	find->right = makeNewAST();
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = NULL;
	vp->pars = makeNewQueue();
			
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	for(int i = 0; i < 2; i++){
		// iterace, ocekavam dva stejny parametry
		
		*cur = getToc();
		if((*cur)->type == T_ID){
			// nacitani z promenne -> tabulka symbolu
			struct symbolTableNode* var = search(&top, (*cur)->data.str);
			if(!var){
				E("Semantic error - undefined variable cannot be used as parameter");
				exit(sem_prog);
			}
		
			if(var->dataType != DT_STR){
				E("Semantic error - INLINE find expected STRING as first parameter");
				exit(sem_komp);
			}
		
			struct String* name;
			copyString((*cur)->data.str, &name);
		
			struct astNode* first = makeNewAST();
			first->type = AST_ID;
			first->dataType = DT_STR;
			copyString((*cur)->data.str, (struct String **)&(first->other));
			queuePush(vp->pars, first);
		}
		else if((*cur)->type == T_STR){
			// nacitani literalu
			struct astNode* first = makeNewAST();
			first->type = AST_STR;
			first->dataType = DT_STR;
			first->data.str = makeNewString();
			copyString((*cur)->data.str, &(first->data.str));
						
			queuePush(vp->pars, first);
		}
		else {
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
			if((*cur)->type != T_COM){
				E("Syntax error - parameters must be separate by comma");
				//printTokenType(*cur);
				exit(synt);
			}
		}
		else if(i == 1){
			// ocekavam pravou zavorku
			if((*cur)->type != T_RPAR){
				E("Syntax error - expected right parenthesis");
				//printTokenType(*cur);
				exit(synt);
			}
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
	vp->vars = NULL;
	vp->pars = makeNewQueue();
	
	// nacist jeden string parametr
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expected left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
		
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
		struct symbolTableNode* var = search(&top, (*cur)->data.str);
		
		if(!var){
			E("Semantic error - variable of parameter not found");
			exit(sem_prog);
		}
	
		// uzel pro promennou
		struct astNode* nd = makeNewAST();
		nd->type = AST_ID;
		nd->dataType = DT_STR;
	
		// jmeno promenne
		struct String* name = NULL;
		copyString(var->name, &name);
		nd->other = name;
	
		queuePush(vp->pars, nd);
	}
	else if((*cur)->type == T_STR){
		// novy uzel pro literal
		struct astNode* nd = makeNewAST();
		nd->type = AST_STR;
		nd->dataType = DT_STR;
		nd->data.str = (*cur)->data.str;
		// pushnuti do fronty
		queuePush(vp->pars, nd);
	}
	else {
		E("Semantic error - expected string as parameter data type");
		exit(sem_komp);
	}
	
	*cur = getToc();
	if((*cur)->type != T_RPAR){
		E("Syntax error - expected right parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->right = makeNewAST();
	node->right->other = vp;
	
	return node;
}
struct astNode* lengthStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_LENGTH;
	node->dataType = DT_INT;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = NULL;
	vp->pars = makeNewQueue();
	
	// nacist jeden string parametr
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expected left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
		
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
		struct symbolTableNode* var = search(&top, (*cur)->data.str);
		
		if(!var){
			E("Semantic error - variable of parameter not found");
			exit(sem_prog);
		}
	
		// uzel pro promennou
		struct astNode* nd = makeNewAST();
		nd->type = AST_ID;
		nd->dataType = DT_STR;
	
		// jmeno promenne
		struct String* name = NULL;
		copyString(var->name, &name);
		nd->other = name;
	
		queuePush(vp->pars, nd);
	}
	else if((*cur)->type == T_STR){
		// novy uzel pro literal
		struct astNode* nd = makeNewAST();
		nd->type = AST_STR;
		nd->dataType = DT_STR;
		nd->data.str = (*cur)->data.str;
		// pushnuti do fronty
		queuePush(vp->pars, nd);
	}
	else {
		E("Semantic error - expected string as parameter data type");
		exit(sem_komp);
	}
	
	*cur = getToc();
	if((*cur)->type != T_RPAR){
		E("Syntax error - expected right parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
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
	vp->vars = NULL;
	vp->pars = makeNewQueue();
	
	*cur = getToc();
	if((*cur)->type != T_LPAR){
		E("Syntax error - expected left parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	
	*cur = getToc();
	if((*cur)->type == T_ID){
		// najit v tabulce symbolu
		struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
		struct symbolTableNode* var = search(&top, (*cur)->data.str);
		if(!var){
			E("Semantic error - variable of parameter not found");
			exit(sem_prog);
		}
					
		// novy uzel
		struct astNode* nid = makeNewAST();
		nid->type = AST_ID;
		nid->dataType = var->dataType;
		
		// kopie jmena
		struct String* name = NULL;
		copyString((*cur)->data.str, &name);
		nid->other = name;
		
		// push prvniho parametru
		queuePush(vp->pars, nid);				
	}
	else if((*cur)->type == T_STR){
		// predelat do nodu
		
		struct astNode* nstr = makeNewAST();
		nstr->type = AST_STR;
		nstr->dataType = DT_STR;
		nstr->data.str = (*cur)->data.str;
		
		queuePush(vp->pars, nstr);				
	}
	else {
		E("Semantic error - expected STRING data type of parameter");
		exit(sem_komp);
	}
	
	*cur = getToc();
	if((*cur)->type != T_COM){
		E("Syntax error - expected comma after parameter");
		//printTokenType(*cur);
		exit(synt);
	}
	
	// ocekavat 2x int
	for(int i = 0; i < 2; i++){
		*cur = getToc();
		
		if((*cur)->type == T_INT){
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
		if(i == 0 && (*cur)->type == T_COM){
			continue;
		}
		else if(i == 1 && (*cur)->type == T_RPAR){
			continue;
		}
		else {
			E("Syntax error - copy parameters error");
			exit(synt);
		}
	}
	
	if((*cur)->type != T_RPAR){
		E("Syntax error - expected right parenthesis");
		//printTokenType(*cur);
		exit(synt);
	}
	
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
				
	*cur = getToc();
	if((*cur)->type != T_INT){
		E("Syntax error - expected int as array index");
		printTokenType(*cur);
		exit(synt);
	}
	// index do datove struktury
	arrid->data.integer = (*cur)->data.integer;
		
	*cur = getToc();
	if((*cur)->type != T_RBRC){
		E("Syntax error - expected right brace after array index");
		printTokenType(*cur);
		exit(synt);
	}
				
	*cur = getToc();
	return arrid;
} 

struct astNode* assignStatement(struct toc** cur){
	// levy uzel je T_ID
	// pravy uzel je expression
	struct astNode* left = makeNewAST();
	if(!left)
		return NULL;					
	left->type = AST_ID;
	copyString((*cur)->data.str, &(left->data.str));
		
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
	//printTokenType(*cur);
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
	D("cmd: Returning asgn node");
	return asgn;
}
/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v tele funkce/programu
 * ------------------------------------------------------------------------------------
 */
struct astNode* parseCommand(struct toc** cur){
	W("parseCommand");
	
	switch((*cur)->type){
		case T_KW_END:
		case T_KW_IF:
		case T_KW_WHILE:
		case T_KW_RPT:
		case T_KW_CASE:
		case T_KW_FOR:
		case T_KW_WRT:
		case T_KW_FIND:
		case T_KW_SORT:
		case T_KW_COPY:
		case T_KW_LENGTH:
		case T_ID:
			break;
		default:
			(*cur) = getToc();
	}
	
	struct astNode* node = NULL;
	struct toc* next;
	
	printTokenType(*cur);
	switch((*cur)->type){
		case T_KW_END:
			// narazil hned na END -> po stredniku nasledoval END chyba 
			// 						  NEBO telo nema prvky
			E("cmd: Syntax error - semicolon after last cmd or body without commands");
			exit(synt);
		case T_KW_IF:
			D("IF command");		
			node = ifStatement(cur);
			if(!node)
				return NULL;
				
			if((*cur)->type == T_KW_END){			
				*cur = getToc();
				return node;
			}
			else {
				E("cmd: Syntax error - expected END (if)");
				//printTokenType(*cur);
				exit(synt);
			}	
			break;
		case T_KW_WHILE:
			D("WHILE command");
			node = whileStatement(cur);
			if(!node)
				return NULL;
			
			if((*cur)->type == T_KW_END){
				*cur = getToc();
				return node;			
			}
			else {
				E("cmd: Syntax error - expected END (while)");			
				//printTokenType(*cur);
				exit(synt);
			}
			break;
		case T_KW_RPT:
			D("REPEAT command");
			node = repeatStatement(cur);
			if(!node)
				return NULL;
			
			if((*cur)->type == T_KW_END){
				*cur = getToc();
				return node;
			}
			else {
				E("cmd: Syntax error - expected END (repeat)");
				//printTokenType(*cur);
				exit(synt);
			}
			break;
		case T_KW_CASE:
			D("CASE cmd");
			node = caseStatement(cur);
			
			if((*cur)->type == T_KW_END){
				*cur= getToc();
				return node;
			}
			else {
				E("cmd: Syntax error - expected END (case)");
				// TODO smazat
				printTokenType(*cur);
				exit(synt);
			}
			break;
		case T_KW_FOR:
			/* for(asgn, int, int) */
			D("FOR cmd");
			
			node = forStatement(cur);
			if((*cur)->type == T_KW_END){
				*cur = getToc();
				return node;
			}
			else {
				E("cmd: Syntax error - expected END (for)");
				printTokenType(*cur);
				exit(synt);
			}		
			break;	
		case T_KW_WRT:
			/* write(params) */
			D("WRITE");
			node = writeStatement(cur);
			return node;
		case T_KW_READLN:
			/* readln(id) */
			D("READLN");
			node = readlnStatement(cur);
			
			return node;
		case T_KW_FIND:
			/* find(str, str) */
			D("FIND");
			node = findStatement(cur);		
			
			return node;
		case T_KW_SORT:
			node = sortStatement(cur);
			return node;
		case T_KW_LENGTH: 
			node = lengthStatement(cur);
			return node;
		case T_KW_COPY:
			D("COPY");
			/* copy(str, int, int) */
			node = copyStatement(cur);
			return node;
		case T_ID:
			next = getToc();
			if(next->type == T_LPAR){
				// pravdepodobne volani funkce
				D("Probably func call");
				//printTokenType(*cur);
				return parseFuncCall(cur);
			}
			else if(next->type == T_LBRC){
				// index poley				
				return arrayIndexStatement(cur);				
			}
			else if(next->type == T_ASGN){
				// pravdepodobne prirazeni
				D("cmd: cur and next token");
				//printTokenType(*cur);
				//printTokenType(next);				
				return assignStatement(cur);
			}
			else {
				E("cmd: Syntax error - expected funcCall or assign");
				//printTokenType(*cur);
				exit(synt);
			}		
			break;
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
	

	// zacatek podminky
	(*cur) = getToc();

// spinavy hack ktory riesi problem old_cur := cur();
	switch((*cur)->type)
	{
	case T_KW_COPY:
	case T_KW_READLN:
	case T_KW_FIND:
	case T_KW_SORT:
	case T_KW_LENGTH:
		{
		struct astNode *cmd = parseCommand(cur);
		stackPush(aststack, cmd);
//		if((*cur)->type == T_KW_END){
//			D("body: gets END");
//			// ukonceni bloku kodu
//			if(cmd == NULL){
//				W("body: WARNING - statement without effect");
//
//				body->left = NULL;
//				return body;
//			}
//
//			body->left = makeNewAST();
//			body->left->type = AST_CMD;
//			body->left->left = NULL;
//			body->left->right = cmd;
//			return body;S
		}
	default: break;
	}
		
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
			stackPush(aststack, nd);
		}
		else {
			D("expr: VARIABLE");
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
#ifdef _DEBUG
		fprintf(stderr, "readNew type = %d \n", readNew);
#endif		
		//printTokenType(*cur);
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
					struct toc* new = getToc();
					if(new->type == T_LPAR){
						// volani funkce
						node = parseFuncCall(cur);	
					}
					else {
						// promenna			
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
						*cur = new;
					}
					// nahrat nacteny token
					
					D("READ NEW - cur type");
					//printTokenType(*cur);
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
					D("expr: STACK STATE START =======");
				
					struct toc* now = (struct toc*)stackPop(stack);
					//printTokenType(now);
					
					
					if(!makeAstFromToken(now, &aststack))
						return NULL;
						
					D("expr: STACK STATE END =========");
					printAstStack(aststack);
				}
				
			
				// v pripade, ze je v zasobniku jen jeden prvek, vratit ho, je to vysledek SY algoritmu
				if(aststack->Length > 1){
					E("expression: Shunting yard error - stack length is grather then 1");
					printAstStack(aststack);
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
	return NULL;
}
