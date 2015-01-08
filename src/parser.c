/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativní­ho jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antoní­n    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juří­k Martin     <xjurik08@stud.fit.vutbr.cz           ****
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
	{"T_KW_UNTIL",T_KW_UNTIL},
	{"T_ID",T_ID},
	{"T_TEXT",T_TEXT},
	{"T_NMB",T_NMB},
	{"T_INT", T_INT},
	{"T_REAL", T_REAL},
	{"T_STR", T_STR},
	{"T_BOOL", T_BOOL},
	{"T_ARR", T_KW_ARRAY},
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

int expect(struct toc* cur, int type, int exitcode){
	if((int)cur->type != type){
		char str[100];
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

/**
 *	Pomocna debugovaci funkce porovnavajici datove typy, ktere se maji rovnat.
 *	Vypisuje barevne podle vysledku porovnani.
 */
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

/**
 *	Hlavni validacni funkce pro zjisteni, zda-li je operace proveditelna nad
 * zadanymi parametry. Pokud by nebyla vraci prislusnou hlasku.
 */
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
	else if(left->type == AST_ARR){
		struct astNode* idnode = NULL;
		if(left->left != NULL)
			idnode = left->left;
		else {
			E("Internal error - wrong structure of nodes");
			exit(intern);
		}
		struct symbolTableNode* arr = searchOnTop(idnode->data.str);
		struct dataTypeArray* dta = (struct dataTypeArray*)arr->other;
		
		if(left->dataType != dta->type){
			left->dataType = dta->type;
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
	else if(right->type == AST_ARR){
		struct astNode* idnode = NULL;
		if(right->left != NULL)
			idnode = right->left;
		else {
			E("Internal error - wrong structure of nodes");
			exit(intern);
		}
		struct symbolTableNode* arr = searchOnTop(idnode->data.str);
		struct dataTypeArray* dta = (struct dataTypeArray*)arr->other;
		if(right->dataType != dta->type){
			right->dataType = dta->type;
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
	
	// porovnani datovych typu u operandu
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
				datatypes(left->dataType, right->dataType);
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
				return DT_BOOL;
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
 * Doprovodna funkce prevadejici token na ast node.
 * ------------------------------------------------
 * V pripade T_ID rychle vytvoreni AST_ID.
 * V pripade operaci vytvari pro ne zastupujici AST nody.
 */
struct astNode* makeNodeFromToken(struct toc** toc){
	struct astNode* node = makeNewAST();

	struct symbolTableNode* var;

	switch((*toc)->type){
		case T_ID:
			var = searchOnTop((*toc)->data.str);
			node->type = AST_ID;
			node->dataType = var->dataType;
			copyString(var->name, &(node->data.str));

			if(node->dataType == DT_ARR){
				node->other = var->other;
			}

			return node;
		case T_EQV:
			node->type = AST_EQV;
			return node;
		case T_NEQV:
			node->type = AST_NEQV;
			return node;
		case T_GRT:
			node->type = AST_GRT;
			return node;
		case T_LSS:
			node->type = AST_LSS;
			return node;
		case T_GEQV:
			node->type = AST_GEQV;
			return node;
		case T_LEQV:
			node->type = AST_LEQV;
			return node;
		case T_ADD:
			node->type = AST_ADD;
			return node;
		case T_SUB:
			node->type = AST_SUB;
			return node;
		case T_OR:
			node->type = AST_OR;
			return node;
		case T_XOR:
			node->type = AST_XOR;
			return node;
		case T_MUL:
			node->type = AST_MUL;
			return node;
		case T_DIV:
			node->type = AST_DIV;
			return node;
		case T_AND:
			node->type = AST_AND;
			return node;
		case T_NOT:
			node->type = AST_NOT;
			return node;
		case T_INT:
			node->type = AST_INT;
			return node;
		case T_REAL:
			node->type = AST_REAL;
			return node;
		case T_BOOL:
			node->type = AST_BOOL;
			return node;
		case T_STR:
			node->type = AST_STR;
			return node;	
	}

	return NULL;
}
/**
 * Vytvari novy uzel do stromu podle operatoru.
 * --------------------------------------------
 * token: Urcuje typ jakeho by mel byl uzel AST
 * aststack: Urcuje zasobnik ze ktereho bude tahat data a kam bude ukladat vysledek
 */
void makeAstFromToken(struct toc* token, struct stack** aststack){
	struct astNode* node = makeNewAST();
	// vytazeny operator
	int prio = getPriority(token);
	if(prio >= 1 && prio <= 3){
		// operatory bez NOT
		node->type = convertTokenToNode(token);
		if((int)node->type == -1){
			E("Unsuported token type to convert");
			exit(synt);
		}
		
		node->right = (struct astNode*)stackPop(*aststack);
		node->left = (struct astNode*)stackPop(*aststack);
		
		// kontrola validity operace a operandu
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
	else if(prio == 4){
		// not
		node->type = AST_NOT;
		node->dataType = DT_BOOL;
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
		exit(synt);						
	}
	
	// ulozeni zpatky na zasobnik
	stackPush(*aststack, node);
}

/**
 * Prevadi klicova slova datovych typu na strukturu DT.
 */
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
		case T_KW_ARRAY:
			return DT_ARR;
		default:
			E("Syntax error - expected supported data type");
			exit(synt);
	}
	// pouze pro prekladac
	return DT_NONE;
}

/**
 * Hlavni funkce parseru - spousti rekurzivni pruchod
 * --------------------------------------------------
 */
int parser(){
	global.symTable->Top = NULL;
	
	D("PARSER START");
	struct astNode* prog = parseProgram();
	if(!prog){
		return False;
	}
	
	// prohledat celou tabulku funkci a hledat polozku bez tela
	checkFunctionDeclarations(global.funcTable);
	
	global.globalTable = NULL;
	
	global.program = prog;
	return True;
}

/**
 * Kontrola deklaraci funkci, jestli maji vsechny forward deklarace
 * svoje definice.
 */
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
		vp->pars = makeNewQueue();
		// lokalni promenne
		program->other = vp;
	}
	
	// prochazet definice funkci a vkladat je s telem do tabulky symbolu
	while(cur->type == T_KW_FUNC){
		D("Another function");
		struct astNode* func = parseFunction();				
		if(!func)
			return NULL;
		
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
	expect(cur, T_KW_END, synt);
	// ocekavat za ENDem tecku
	cur = getToc();
	expect(cur, T_DOT, synt);
	// ocekavat EOF
	cur = getToc();
	expect(cur, T_EOF, synt);
			
	return program;
}

/**
 * Parsuje telo. Predpoklada, ze begin uz byl nacten a end vrati posledni
 * prikaz tela
 * ---------------------------------------------------------------------
 * @param cur Odkaz na strukturu tokenu, pro predani zpet posledniho tokenu
 * @param empty Pokud muze byt telo prazdne pak true
 * @param ocekavany koncovy typ, vetsinou T_KW_END
 */
struct astNode* parseBody(struct toc** cur, bool empty, int endtype){
	W("parseBody");
	
	// zacatek tela -> spustit prikazy
	// ukladat je doleva za sebe do typu AST_CMDparseBody
	struct astNode* body = makeNewAST();
	body->type = AST_CMD;
		
	*cur = getToc(); // prvni token tela!!!
	if((int)(*cur)->type == endtype){
		D("Empty body");	
		if(empty)
			return body;
		else {
			E("Syntax error - empty body is not supported in this case");
			exit(synt);		
		}
	}
	
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
			// pokud narazi na END tak ukoncit hledani dalsich 
			if((int)((*cur)->type) == endtype){
				D("Gets endtype");
				break;
			}
			else if((*cur)->type == T_SCOL){
				// nutno nacist dalsi token
				*cur = getToc();
				// hledat dalsi
				cmd = parseCommand(cur);
				// posun na dalsi pole v seznamu prikazu
				item = item->left;
			}
			else {
				E("body: Syntax error - expected END or SEMICOLON");
				exit(synt);			
			}			
		}
	}

	W("parseBody end");
	return body;
}

/**
 *	Funkce pro sekvencni zpracovani definice pole.
 * -----------------------------------------------
 * @param cur Odkaz na token z vyssi vrstvy
 * @param name Predane jmeno z duvodu nacitani dalsich tokenu
 */
struct astNode* getArrayDef(struct toc** cur, struct String* name){
	struct astNode* node = makeNewAST();
	struct dataTypeArray* dta = NULL;
	
	// typ pole
	node->type = AST_ARR;
	node->dataType = DT_ARR;

	dta = (struct dataTypeArray*)gcMalloc(sizeof(struct dataTypeArray));
	if(!dta)
		return NULL;
	dta->id = name;
	dta->data = NULL;
	// ulozi se jmeno
	node->data.str = name;

	(*cur) = getToc();
	expect((*cur), T_LBRC, synt);
	// leva [ -> ocekavat integer								
	(*cur) = getToc();
	expect(*cur, T_INT, sem_else);
	dta->low = (*cur)->data.integer;
	
	// interval - dve tecky mezi integery
	(*cur) = getToc();
	expect(*cur, T_DDOT, synt);
	
	// ocekavat druhy integer
	(*cur) = getToc();
	expect(*cur, T_INT, sem_else);
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
	if(dta->type == DT_ARR){
		E("Syntax error - unsupported array as type of array item");
		exit(sem_else);
	}
	
	// ulozeni odkazu na strukturu dat
	node->other = dta;		
								
	return node;
}

/**
 *	Ocekava nacteni ID z definici.
 * -------------------------------
 * @param cur Odkaz na token z vyssi vrstvy
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
	else if(node->dataType == DT_ARR){
		// nacitani pole
		node = getArrayDef(cur, node->data.str);
	}
	
	// kazda promenna musi koncit strednikem
	*cur = getToc();
	expect(*cur, T_SCOL, synt);
	
	return node;
}

/**
 * Parsuje var sekci funkce/programu
 * ---------------------------------
 * @param cur Odkaz na token z vyssi vrstvy
 */
struct queue* parseVars(struct toc** cur){
	W("parseVars");

	// zacinat parsovat promenne
	struct queue* vars = makeNewQueue();		
	struct symbolTableNode* top = (struct symbolTableNode*)stackPop(global.symTable);
	
	struct astNode* var = NULL;
	struct dataTypeArray* dta = NULL;
	
	*cur = getToc();
	expect(*cur, T_ID, synt);
	while((*cur)->type == T_ID){
		// nacteni definice
		var = getVarDef(cur);
		if(var->type == AST_ARR){
			dta = (struct dataTypeArray*)var->other;
		}	
		
		// ulozit do top vrstvy
		if(search(&global.globalTable, var->data.str)){
			E("Promenna nalezena v globalni tabulce");
			exit(sem_prog);
		}
		else {
			struct symbolTableNode* inserted = insertValue(&top, var->data.str, var->dataType);
			if(var->type == AST_ARR && dta != NULL){
				inserted->other = dta;	
			}
		}

		// vlozit prvek
		queuePush(vars, var);
		
		// nacist dalsi token
		*cur = getToc();
	}

	// ulozeni nove vrstvy
	stackPush(global.symTable, top);
			
	// vratit seznam promennych
	return vars;
}

/**
 *	Pasruje parametry definice funkce, podle tabulky.
 * -------------------------------------------------
 * @param cur Odkaz na token z vyssi vrstvy
 */
struct astNode* getDefPar(struct toc** cur){
	// parsuje def_par pouze:
	// id:type
	
	struct astNode* par = makeNewAST();
	par->type = AST_ID;
	
	// nacteni prvniho tokenu
	*cur = getToc();
	
	// konec parametru -- nejedna se o chybu!
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
	return params;		
}

/**
 * Parsuje dopredne deklarace / definice funkci
 * --------------------------------------------
 * Jelikoz funkce zacina kw FUNCTION a konci strednikem,
 * neni nutne predavat odkaz na token.
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

	// kontroly na redefinice
	switch(cur->type){
		case T_KW_SORT:
		case T_KW_FIND:
		case T_KW_COPY:
		case T_KW_LENGTH:
		case T_KW_WRT:
		case T_KW_READLN:
			E("Semantic error - redefinition of inline function");
			exit(sem_prog);
		case T_ID:
			break;
		default:
			E("Syntax error - expected ID");
			exit(synt);
	}
	// ocekavani nazvu
	expect(cur, T_ID, synt);
	
	// skopirovani jmena
	struct String* name = makeNewString();
	copyString(cur->data.str, &name);
	node->other = name;

	// funkce -- jako promenna v globalni tabulce
	if(global.globalTable != NULL){
		if(search(&global.globalTable, name)){
			E("Semantic error - function name found as variable");
			exit(sem_prog);
		}			
	}
	struct symbolTableNode* top = (struct symbolTableNode*)stackTop(global.symTable);
	if(search(&top, name)){
		E("Semantic error - function name found as variable");
		exit(sem_prog);
	}

	// nacteni leve zavorky na zacatek parametru
	cur = getToc();
	expect(cur, T_LPAR, synt);

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
	//	KONEC HLAVICKY FUNKCE

	// deklarace funkce
	struct symbolTableNode* dekl = (struct symbolTableNode*)search(&global.funcTable, name);
	
	// DOPREDNA DEKLARACE FUNKCE
	cur = getToc();
	if(cur->type == T_KW_FORW){
		D("FORWARD DEC!");
				
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
		deleteTable(&global.globalTable);
						
	 	// nastaveni navratovy typ
	 	dekl->dataType = node->dataType;
	 	// jedna se o deklaraci - tedy init = false
	 	dekl->init = false;
	 		 	
	 	// pravy uzel s parametry funkce
	 	node->right = makeNewAST();
	 	node->right->other = vp;
	 	// nastaveni parametru do funkce
	 	dekl->other = node;
	 	
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

			// po definici promennych MUSI nasledovat telo, tedy begin
			expect(cur, T_KW_BEGIN, synt);
		}
		node->right = makeNewAST();
		node->right->other = vp;
		
		// funkce byla definovana - tedy je inicializovana
		// hack pro podporu rekurze
		dekl->init = true;
		// ulozeni do tabulky symbolu
		dekl->other = node;	
		
		// ocekavat telo
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
 * @param cur Odkaz na token z vyssi vrstvy
 */
struct queue* parseCallParams(struct toc** cur){
	W("parseCallParams");
	// prochazi podel parametru a vytvari doleva jdouci strom z parametru
	struct queue* pars = makeNewQueue();
	struct toc* next = NULL;
	bool readNew = true;

	(*cur) = getToc();
	while((*cur)->type != T_RPAR){
		readNew = true;
		struct astNode* nd = makeNewAST();

		struct symbolTableNode* id = NULL;
		switch((*cur)->type){
			case T_ID:
				// predana promenna
				next = getToc();
				if(next->type == T_LBRC){
					// array index
					nd = arrayIndex(cur, (*cur)->data.str);
					break;
				}
				
				// vyhledani promenne v tabulkach
				id = searchOnTop((*cur)->data.str);
				// nastaveni typu
				nd->type = AST_ID;
				// kopie jmena promenne
				copyString((*cur)->data.str, &(nd->data.str));
				// kopie datoveho typu
				nd->dataType = id->dataType;
				datatypes(nd->dataType, id->dataType);
				// pokud se nejednalo o [
				
				// prenastavit tokeny
				readNew = false;
				*cur = next;
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
				exit(synt);
		}
		
		// nacteni oddelovace
		if(readNew)
			(*cur) = getToc();
		if((*cur)->type == T_COM){
			// nacetl carku - v poradku pokud nenasleduje T_RPAR
			(*cur) = getToc();
			if((*cur)->type == T_RPAR){
				E("callPars: Syntax error - comma before right parenthesis");
				exit(synt);
			}
		}
		
		// vytvoreni polozky a vlozeni polozky do node
		D("Pushing into call queue");
		queuePush(pars, nd);
	}
	
	D("Expected data - no received");
	return pars;
}
/**
 * Porovnava data z definovanch parametru s predanymi
 * --------------------------------------------------
 * @param callParams: parametry z volani
 * @param funcParams: parametry z definice
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
	
	struct queueItem* qi1 = NULL;
	struct queueItem* qi2 = NULL;
	qi1 = callParams->start;
	qi2 = funcParams->start;

	struct astNode* qi1ast = NULL;
	struct astNode* qi2ast = NULL;
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
 * -------------------------------------------------------
 * @param defPars: Parametry z definice
 * @param decPars: Parametry z dopredne deklarace
 */
void controlDefinitionParams(struct queue* defPars, struct queue* decPars){
	W("controlDefinitionParams");
	if(defPars == NULL || decPars == NULL){
		E("control: NULL parameters queue");
		exit(intern);
	}
	
	struct queueItem* defs = defPars->start;
	struct queueItem* decs = decPars->start;

	struct astNode* def = NULL;
	struct astNode* dec = NULL;
	struct String* defname = NULL;
	struct String* decname = NULL;
	
	while(decs != NULL){
		def = (struct astNode*)defs->value;
		dec = (struct astNode*)decs->value;
	
		defname = def->data.str;
		decname = dec->data.str;
				
		// porovnani nazvu promennych
		if(compareStrings(defname, decname) != 0){
			E("Semantic error - expected same parameter name");
			exit(sem_prog);
		}
	
		// porovnani datovych typu
		if(def->dataType != dec->dataType){
			// nesedi datove typy
			E("Semantic error - expected same parameter type");
			exit(sem_prog);
		}
	
		defs = defs->next;
		decs = decs->next;
		if(decs != NULL){
			if(defs == NULL){
				// malo parametru predanych
				E("Semantic error - few parameters");
				exit(sem_prog);
			}
			else {
				continue;
			}
		}
		else {
			if(defs != NULL){
				// chybi parametry v definici
				E("Semantic error - too many parameters");
				exit(sem_prog);
			}
		}
	}	
}

/**
 * Parsuje volani funkce, kde uz dostane predany T_ID od exprParseru
 * -----------------------------------------------------------------
 * @param id: Token ID se jmenem funkce
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

	// kontrola parametru
	controlCallParams(vp->pars, vpfunc->pars);
	node->left = NULL;

	// nacteni posledniho tokenu za volanim
	*id = getToc();
	
	return node;
}

/**
 * Parsuje IF statement	
 * -----------------------------------------------------------------
 * Predpoklada nacteny token T_KW_IF.
 * @param cur: odkaz na token z vyssi vrstvy
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

	// za podminkou ocekavat THEN
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
 * Parsuje WHILE cyklus
 * -----------------------------------------------------------------
 * Predpoklada nacteny token T_KW_WHILE
 * @param cur: Odkaz na token z vyssi vrstvy
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
 * ----------------------------------------------------------------
 * Implementovane rozsireni. Implicitne cyklus nepotrebuje begin-end,
 * staci pouze mezi repeat-until vlozit prikazy.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* repeatStatement(struct toc** cur){
	struct astNode* rpt = makeNewAST();
	if(!rpt)
		return NULL;
	rpt->type = AST_REPEAT;
	// telo cyklu - dokud nenarazi na UNTIL
	rpt->left = parseBody(cur, false, T_KW_UNTIL);
	if(!rpt->left)
		return NULL;
	
	// nacetl UNTIL
	struct astNode* tmp = parseExpression(cur);	
	if(tmp->dataType != DT_BOOL){
		E("Semantic error - if condition has not BOOL data type");
		exit(sem_komp);
	}
	rpt->other = tmp;
	// expresion se zastavi na tom tokenu ktery nezna	
	// ten se vrati
	return rpt;
}
/**
 * Parsuje FOR cyklus
 * ----------------------------------------------------------------
 * Implementovane rozsireni. Cyklus ma dva mody chodu, to/downto,
 * kdy v kazdem je potreba promenna, do ktere se prirazuje a ktera se
 * dekrementuje nebo inkrementuje.
 * 
 * @param cur: Odkaz na token z vyssi vrstvy
 */
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
	
	// nacist pravou stranu jako vyraz
	forCond->right = parseExpression(cur);
	if(forCond->right->dataType != DT_INT){
		E("Semantic error - expected integer as right side of assign in for");
		exit(sem_komp);
	}
	
	// nacitat podminku - vyraz vraci posledni nacteny
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
		exit(synt);
	}
	
	// ulozeni doleva prirazeni, doprava pujde koncova hodnota
	forNode->right->left = forCond;
	
	// nacitat literal
	//*cur = getToc();
	forNode->right->right = parseExpression(cur);
	if(forNode->right->right->dataType != DT_INT){
		E("Semantic error - expected integer");
		exit(sem_komp);
	}
	
	// nacteni DO za definici
	expect(*cur, T_KW_DO, synt);

	*cur = getToc();
	expect(*cur, T_KW_BEGIN, synt);
	// ocekavani tela
	forNode->left = parseBody(cur, true, T_KW_END);
	
	// posledni token -> za prikazem
	*cur = getToc();

	return forNode;
}

// INLINE FUNKCE
/**
 * WRITE inline funkce
 * ----------------------------------------------------------------
 * Vnitrni funkce write umoznuje tisk n parametru na stdout. 
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* writeStatement(struct toc** cur){
	struct toc* next = NULL;
	bool readNew = true;
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
		readNew = true;
		
		// ocekavat cokoliv
		struct astNode* nd = makeNewAST();
		struct symbolTableNode* var = NULL;
		
		switch((*cur)->type){
			case T_ID:
				// v pripade ze se jedna o array index
				next = getToc();
				if(next->type == T_LBRC){
					// arrayIndex

					nd = arrayIndex(cur, (*cur)->data.str);
					break;
				}
				
				var = searchOnTop((*cur)->data.str);
				// vytvorit novy uzel
				nd->type = AST_ID;				
				// nazev promenne
				copyString((*cur)->data.str, &(nd->data.str));
				// datovy typ promenne
				nd->dataType = var->dataType;
				
				// nenacitat dalsi token, uz sme nacetli
				readNew = false;
				*cur = next;
							
				break;
			case T_INT: 
				nd->type = AST_INT;
				nd->dataType = DT_INT;
				nd->data.integer = (*cur)->data.integer;
				break;
			case T_REAL: 
				nd->type = AST_REAL;
				nd->dataType = DT_REAL;
				nd->data.real = (*cur)->data.real;						
				break;
			case T_STR: 
				nd->type = AST_STR;
				nd->dataType = DT_STR;
				copyString((*cur)->data.str, &(nd->data.str));
				
				break;
			case T_BOOL: 						
				nd->type = AST_BOOL;
				nd->dataType = DT_BOOL;
				nd->data.boolean = (*cur)->data.boolean;
				break;		
			default:
				E("Syntax error - unsupported type of parameter");
				exit(synt);
		}

		// push do fronty
		queuePush(vp->pars, nd);						
				
		// dalsi token + dopredu
		if(readNew)
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
		else if((*cur)->type == T_RPAR){
			break;
		}
		else {
			// spatna struktura parametru - pravdepodobne pokus o vyraz
			E("Semantic error - wrong param structure");
			exit(sem_komp);
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
/**
 * READLN inline funkce
 * ----------------------------------------------------------------
 * Vnitrni funkce interpretu pro cteni vstupu od uzivatele. Jediny parametr
 * pozadovaneho typu, nesmi byt boolean.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* readlnStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_READLN;
		
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	// pokud nenasleduje ID -> chyba
	*cur = getToc();
	switch((*cur)->type){
		case T_INT:
		case T_REAL:
		case T_BOOL:
		case T_STR:
			E("Semantic error - expected ID as parameter");
			exit(sem_komp);
		case T_ID:
			// v poradku
			break;
		case T_RPAR:
			// bez parametru
			E("Semantic error - expected one ID parameter");
			exit(sem_komp);
		default:
			E("Syntax error - expected only ID token");
			exit(synt);
	}
	
	// jeden parametry - ID
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();

	// vyhledat promennou a zkontrolovat datovy typ
	struct symbolTableNode* var = searchOnTop((*cur)->data.str);
	if(var->dataType == DT_BOOL || var->dataType == DT_ARR){
		E("Semantic error - readln parameter cannot be BOOL or ARRAY");
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
/**
 * FIND inline funkce
 * ----------------------------------------------------------------
 * Funkce pro vyhledavani stringu v textech. Ma dva parametry typu
 * string, muze to byl literal nebo promenna.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* findStatement(struct toc** cur){
	struct astNode* find = makeNewAST();
	struct astNode* nd = NULL;
	struct toc* next = NULL;
 	find->type = AST_FIND;
	find->dataType = DT_INT;

	// nacist zacatek parametru	
	*cur = getToc();
	expect((*cur), T_LPAR, synt);
	
	find->right = makeNewAST();
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();

	bool readNew = true;
	for(int i = 0; i < 2; i++){
		readNew = true;
		// iterace, ocekavam dva stejny parametry		
		*cur = getToc();
		if((*cur)->type == T_ID){
			next = getToc();
			if(next->type == T_LBRC){
				// nactena [
				nd = arrayIndex(cur, (*cur)->data.str);
				
				if(nd->dataType != DT_STR){
					E("Semantic error - INLINE find expected STRING as parameter");
					exit(sem_komp);
				}
			}
			else {
				// nacitani z promenne -> tabulka symbolu
				struct symbolTableNode* var = searchOnTop((*cur)->data.str);
			
				if(var->dataType != DT_STR){
					E("Semantic error - INLINE find expected STRING as parameter");
					exit(sem_komp);
				}
			
				nd = makeNewAST();
				nd->type = AST_ID;
				nd->dataType = DT_STR;
				copyString((*cur)->data.str, &(nd->data.str));

				readNew = false;
				*cur = next;
			}
			// novy parametr
			queuePush(vp->pars, nd);
		}
		else if((*cur)->type == T_STR){
			// nacitani literalu
			struct astNode* first = makeNewAST();
			first->type = AST_STR;
			first->dataType = DT_STR;
			copyString((*cur)->data.str, &(first->data.str));

			// novy parametr
			queuePush(vp->pars, first);
		}
		else if((*cur)->type == T_RPAR){
			E("Semantic error - expexted 2 parameters");
			exit(sem_komp);
		}
		else {
			// vypis kde co ocekaval
			if(i == 0){
				E("Syntax error - INLINE find expected string as first parameter data type");
			}
			else {
				E("Syntax error - INLINE find expected string as second parameter data type");
			}
			exit(sem_komp);
		}
			
		// oddelovac nebo prava zavorka
		if(readNew)
			*cur = getToc();
		if(i == 0){
			// ocekavam carku
			expect((*cur), T_COM, synt);
		}
		else {
			// ocekavam pravou zavorku
			if((*cur)->type == T_COM){
				E("Semantic error - expected at least 2 parameters");
				exit(sem_komp);
			}
			expect((*cur), T_RPAR, synt);
		}
	}
		
	// dalsi token
	*cur = getToc();			
	
	find->right = makeNewAST();
	find->right->other = vp;
	return find;
}
/**
 * SORT inline funkce
 * ----------------------------------------------------------------
 * Funkce pro razeni textu z jedineho parametru. Pouze typ string.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* sortStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	struct astNode* nd = NULL;
	struct toc* next = NULL;
	bool readNew = true;
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
		next = getToc();
		if(next->type == T_LBRC){
			// array index
			nd = arrayIndex(cur, (*cur)->data.str);
			if(nd->dataType != DT_STR){
				E("Semantic error - expected string");
				exit(sem_komp);
			}
		}
		else {
			// obyc promenna
			struct symbolTableNode* var = searchOnTop((*cur)->data.str);

			if(var->dataType != DT_STR){
				E("Semantic error - expected string as parameter data type");
				exit(sem_komp);
			}
			
			// uzel pro promennou
			nd = makeNewAST();
			nd->type = AST_ID;
			nd->dataType = DT_STR;
		
			// jmeno promenne
			copyString(var->name, &(nd->data.str));

			readNew = false;
			*cur = next;
		}
		
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
	else if((*cur)->type == T_RPAR) {
		E("Semantic error - expected string as parameter data type");
		exit(sem_komp);
	}
	else {
		E("Syntax error - expected at least one parameter");
		exit(synt);
	}

	// konec parametru
	if(readNew)
		*cur = getToc();
	expect((*cur), T_RPAR, synt);
	
	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->right = makeNewAST();
	node->right->other = vp;
	
	return node;
}
/**
 * LENGTH inline funkce
 * ----------------------------------------------------------------
 * Funkce pro ziskani delky predaneho textu v parametru. Pouze string.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* lengthStatement(struct toc** cur){
	struct astNode* node = makeNewAST();
	node->type = AST_LENGTH;
	node->dataType = DT_INT;
	bool readNew = true;
	struct toc* next = NULL;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	// nacist jeden string parametr
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	// parametr
	*cur = getToc();
	if((*cur)->type == T_ID){
		struct astNode* nd = NULL;
		
		next = getToc();
		if(next->type == T_LBRC){
			nd = arrayIndex(cur, (*cur)->data.str);
			if(nd->dataType != DT_STR){
 				E("Semantic error - expected string");
 				exit(sem_komp);
			}
		}
		else {
			struct symbolTableNode* var = searchOnTop((*cur)->data.str);
			if(var->dataType != DT_STR){
				E("Semantic error - expected string as parameter data type");
				exit(sem_komp);
			}
			
			// uzel pro promennou
			nd = makeNewAST();
			nd->type = AST_ID;
			nd->dataType = DT_STR;	
			// jmeno promenne
			copyString(var->name, &(nd->data.str));

			readNew = false;
			*cur = next;
		}
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

	if(readNew)
		*cur = getToc();
	expect((*cur), T_RPAR, synt);
	
	// nacteni dalsiho tokenu
	*cur = getToc();
	
	node->right = makeNewAST();
	node->right->other = vp;
	
	return node;
}
/**
 * COPY inline funkce
 * ----------------------------------------------------------------
 * Funkce pro kopirovani casti textu z parametru. Vyrez je urcen dalsima
 * dvema parametry typu integer.
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* copyStatement(struct toc** cur){
	struct astNode* copy = makeNewAST();
	copy->type = AST_COPY;
	copy->dataType = DT_STR;
	bool readNew = true;
	struct toc* next = NULL;
	
	struct varspars* vp = (struct varspars*)gcMalloc(sizeof(struct varspars));
	vp->vars = makeNewQueue();
	vp->pars = makeNewQueue();
	
	*cur = getToc();
	expect((*cur), T_LPAR, synt);

	struct astNode* nid = NULL;
	*cur = getToc();
	if((*cur)->type == T_ID){
		next = getToc();
		if(next->type == T_LBRC){
			nid = arrayIndex(cur, (*cur)->data.str);
			if(nid->dataType != DT_STR){
 				E("Semantic error - expected string");
 				exit(sem_komp);
			}
		}
		else {			
			// najit v tabulce symbolu
			struct symbolTableNode* var = searchOnTop((*cur)->data.str);
			if(var->dataType != DT_STR){
				E("Semantic error - expected string");
				exit(sem_komp);
			}
			
			// novy uzel
			nid = makeNewAST();
			nid->type = AST_ID;
			nid->dataType = var->dataType;		
			// kopie jmena
			copyString((*cur)->data.str, &(nid->data.str));

			readNew = false;
			*cur = next;
		}
		
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
	else if((*cur)->type == T_RPAR){
		// bez parametru
		E("Semantic error - expected 3 parameters, gets 1");
		exit(sem_komp);		
	}
	else {
		E("Semantic error - expected STRING data type of parameter");
		exit(sem_komp);
	}

	if(readNew)
		*cur = getToc();
	expect((*cur), T_COM, synt);

	// dycky nutno nastavit
	readNew = true;
	
	// ocekavat 2x int
	for(int i = 0; i < 2; i++){
		*cur = getToc();

		if((*cur)->type == T_ID){
			next = getToc();
			if(next->type == T_LBRC){
				nid = arrayIndex(cur, (*cur)->data.str);
				if(nid->dataType != DT_INT){
					E("Semantic error - expected integer");
					exit(sem_komp);
				}
			}
			else {
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

				readNew = false;
				*cur = next;
			}
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
		
		if(readNew)
			*cur = getToc();
		if(i == 0)
			expect((*cur), T_COM, synt);
		else{
			if((*cur)->type == T_COM){
				E("Semantic error - expected at least 2 parameters");
				exit(sem_komp);
			}
			expect((*cur), T_RPAR, synt);
		}
	}
	
	expect((*cur), T_RPAR, synt);

	// nacteni posledniho tokenu
	*cur = getToc();
	
	copy->right = makeNewAST();
	copy->right->other = vp;
	return copy;	
}

/**
 *	Parsuje prikaz prirazeni podle LL tabulky
 * ----------------------------------------------------------------
 * ASSIGN       ->	id := AFTER_ASSIGN
 * AFTER_ASSIGN	->	EXPR
 * AFTER_ASSIGN	->	CALL
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* assignStatement(struct toc** cur){
	// levy uzel je T_ID -- z parse command
	// pravy uzel je expression
	struct astNode* left = makeNodeFromToken(cur);	
		
	// skopirovani informaci z tabulky symbolu
	struct symbolTableNode* nd = searchOnTop(left->data.str);				
	left->dataType = nd->dataType;
	
	struct astNode* right = parseExpression(cur);		
	
	if(left->dataType != right->dataType){
		datatypes(left->dataType, right->dataType);
		E("cmd: Semantic error - L-value has not same value as R-value");
		exit(sem_komp);
	}
	if(left->dataType == DT_ARR){
		struct dataTypeArray* dtaL = (struct dataTypeArray*)left->other;
		struct dataTypeArray* dtaR = (struct dataTypeArray*)right->other;

		if(dtaL->type != dtaR->type){
			E("Semantic error - expected same data types in array copy");
			exit(sem_komp);
		}
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
 * Parsuje index pole, kontroluje existenci zadane promenne a zjistuje
 * jestli se jedna o pole.
 * ----------------------------------------------------------------
 * @param cur: Odkaz na token z vyssi vrstvy
 * @param name: Predpoklada ze se nacetl T_ID a nasledne T_LBRC, proto
 * potreba predat jmeno z T_ID
 */
struct astNode* arrayIndex(struct toc** cur, struct String* name){
	W("arrayIndex");
	struct astNode* node = NULL;	
	// LBRC ma nactene
	
	// kontrola, jestli je promenna pole
	struct symbolTableNode* var = searchOnTop(name);
	if(var->dataType != DT_ARR){
		E("Semantic error - L-value is not ARRAY");
		exit(sem_komp); 
	}
	// ziskani potrebne struktury
	struct dataTypeArray* dta = (struct dataTypeArray*)var->other;

	// hlavni node 
	node = makeNewAST();
	node->type = AST_ARR;
	// nastavi se datovy typ pro obsah
	node->dataType = dta->type;
	node->other = dta;
	
	// do leveho stromu ulozit promennou
	node->left = makeNewAST();
	node->left->type = AST_ID;
	node->left->dataType = dta->type;
	node->left->data.str = name;

	D("Left node done");
	// ocekavat INT jako index (nebo ID!!)
	*cur = getToc();
	if((*cur)->type == T_ID){
		// index byla promenna - ziskat typ
		// a nastavit odkaz do other
		node->right = makeNodeFromToken(cur);
		if(node->right->dataType != DT_INT){
			E("Semantic error - index must be integer");
			exit(sem_else);
		}
	}
	else if((*cur)->type == T_INT){
		// v pripade ze index byl literal
		int value = (*cur)->data.integer;
		if(dta->low <= value && value <= dta->high) {
			node->right = makeNewAST();
			node->right->type = AST_INT;
			node->right->dataType = DT_INT;
			node->right->data.integer = value;
		}
		else {
			E("Semantic error - index out of range");
			exit(sem_else);		
		}					
	}
	else if((*cur)->type == T_RBRC){
		// nema index
		E("Syntax error - expected integer as index, no found");
		exit(synt);
	}
	else // exit
		expect(*cur, T_INT, sem_else);			
	D("Right node done");

	// ocekavat konec zavorek
	*cur = getToc();
	expect(*cur, T_RBRC, synt);

	return node;
}
/**
 * Parsuje prikaz prirazeni pro index do pole.
 * ----------------------------------------------------------------
 * @param cur: Odkaz na token z vyssi vrstvy
 * @param name: Predpoklada ze se nacetl T_ID a nasledne T_LBRC, proto
 * potreba predat jmeno z T_ID
 */
struct astNode* arrayAssignStatement(struct toc** cur, struct String* name){
	struct astNode* node = makeNewAST();

	// ziskani nodu pro ARRID
	node->left = arrayIndex(cur, name);	
	
	// ocekavat :=
	*cur = getToc();
	expect(*cur, T_ASGN, synt);
	node->type = AST_ASGN;

	// expression napravo
	node->right = parseExpression(cur);		
	
	if(node->left->dataType != node->right->dataType){
		datatypes(node->left->dataType, node->right->dataType);
		E("assignStatement: Semantic error - L-value has not same value as R-value");
		exit(sem_komp);
	}

	node->left->data.str = name;
	return node;
}

/**
 * Vyhodnoti vsechny moznosti prikazu, ktere se muzou vyskytovat v
 * tele funkce/programu. Pro kazdy mozny prikaz v tele je spoustena
 * odpovidajici funkce.
 * -----------------------------------------------------------------
 * @param cur: Odkaz na token z vyssi vrstvy
 */
struct astNode* parseCommand(struct toc** cur){
	struct astNode* node = NULL;
	struct toc* next = NULL;
	
	// prvni token je nacteny
	switch((*cur)->type){
		case T_KW_BEGIN:
			// dalsi blok kodu
			node = parseBody(cur, true, T_KW_END);
			// nacist dalsi token
			*cur = getToc();
			return node;
		case T_KW_IF:	
			node = ifStatement(cur);
			if(!node)
				return NULL;

			return node;
		case T_KW_WHILE:
			node = whileStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_RPT:
			node = repeatStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_FOR:			
			node = forStatement(cur);
			if(!node)
				return NULL;
			return node;	
		case T_KW_WRT:
			node = writeStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_KW_READLN:
			node = readlnStatement(cur);
			if(!node)
				return NULL;
			return node;
		case T_ID:
			// podle next tokenu se rozlisi o co jde
			next = getToc();
			if(next->type == T_LBRC){
				// nacetl [ ocekava se index pole --> cele prirazeni
				node = arrayAssignStatement(cur, (*cur)->data.str);
			}
			else {
				// pokud nebude [ pak ocekavat asgn
				expect(next, T_ASGN, synt);
				// v cur je ID
				node = assignStatement(cur);
			}
			return node;
		default:
			E("Syntax error - unexpected token type");
			exit(synt);
	}
	return NULL;
}

/**
 * Parsuje vyraz, aritmeticko-logicky. Implementace Shunting-yard algoritmu.
 * Zpracovava tokeny podle jejich typu a priorit a primo generuje AST.
 * ------------------------------------------------------------------
 * @param cur: odkaz na token z vyssi vrstvy
 */
struct astNode* parseExpression(struct toc** cur){
	W("parseExpression");
	// zasobnik pro operatory Shunting-yard algoritmu
	struct stack* stack = makeNewStack();
	// zasobnik pro chystani AST
	struct stack* aststack = makeNewStack();

	bool readNew = true;
	//struct astNode *cmd = NULL;
	int last = -1;

	// zacatek podminky
	(*cur) = getToc();	

	struct toc* t = NULL;
	struct toc* top = NULL;
	struct toc* now = NULL;
	struct astNode* node = NULL;
	while((*cur) != NULL){
		switch((*cur)->type){
			case T_KW_LENGTH:
				node = lengthStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_SORT:
				node = sortStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_FIND:
				node = findStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			case T_KW_COPY:
				node = copyStatement(cur);
				stackPush(aststack, node);
				readNew = false;
				break;
			// leva zavorka
			case T_LPAR: 
				stackPush(stack, (*cur));
				break;
			// prava zavorka
			case T_RPAR:
				t = (struct toc*)stackPop(stack);
				if(t == NULL){
					// pravdepodobne chyba syntaxe
					E("expression: Syntax error - no Left parenthesis before Right parenthesis");
					exit(synt);
				}
				
				while(!stackEmpty(stack) && (t->type != T_LPAR)){
					// vybira operatory ze zasobniku a hrne je do zasobniku operandu
					makeAstFromToken(t, &aststack);
					
					t = (struct toc*)stackPop(stack);				
					// popnul vsechno do leve zavorky
				}				
				break;
			// operandy
			case T_ID:
				node = makeNewAST();
				// muze se objevit ID nebo FUNC call

				// nacist dalsi token
				t = getToc();
				if(t->type == T_LPAR){
					// pokud nenajde funkci jedna se o syntax error
					if(!search(&global.funcTable, (*cur)->data.str)){
						E("Semantic error - undefined function");
						exit(sem_prog);
					}
					// funkci nasel, spustit parsing
					node = parseFuncCall(cur);
					// parseFuncCall uz nacetl posledni znak
					readNew = false;					
					// funkci ulozit do vystupu
					stackPush(aststack, node);
					break;
				}
				else if(t->type == T_LBRC){
					// pokus o index do pole...
					node = arrayIndex(cur, (*cur)->data.str);
					// nacetl posledni ] takze cist dalsi
					readNew = true;
					// ulozit navrchol
					stackPush(aststack, node);
					break;
				}

				// v pripade ze dalsi token nebyla zavorka
				// z ID udelat promennou
				node = makeNodeFromToken(cur);
				stackPush(aststack, node);
				// jelikoz mame nacteny dalsi token
				readNew = false;
				// nahrat token co cur
				*cur = t;
				break;
			case T_INT:
			case T_REAL:
			case T_STR:
			case T_BOOL: 
			case T_KW_TRUE:
			case T_KW_FALSE:
				node = makeNewAST();
				
				if((*cur)->type == T_INT){
					node->type = AST_INT;
					node->dataType = DT_INT;
					node->data.integer = (*cur)->data.integer;			
				}
				else if((*cur)->type == T_REAL){
					node->type = AST_REAL;
					node->dataType = DT_REAL;
					node->data.real = (*cur)->data.real;
				}
				else if((*cur)->type == T_BOOL){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = (*cur)->data.boolean;
				}
				else if((*cur)->type == T_STR){
					node->type = AST_STR;						
					node->dataType = DT_STR;
					copyString((*cur)->data.str, &(node->data.str));
				}
				else if((*cur)->type == T_KW_TRUE){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = true;
				}
				else if((*cur)->type == T_KW_FALSE){
					node->type = AST_BOOL;
					node->dataType = DT_BOOL;
					node->data.boolean = false;					
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
				if(last == T_LPAR){
					// pred operatorem byla leva zavorka
					E("Syntax error - wrong syntax, left parenthesis before operator");
					exit(synt);
				}
				
				// 3. operator -> vlozit
				top = (struct toc*)stackTop(stack);
				// 	pokud je zasobnik prazdny -- vubec neprojde whilem
				while(!stackEmpty(stack)){
					// 	pokud je na vrcholu leva zavorka
					// 	pokud je na vrcholu operator s nizsi prioritou
					if(top == NULL || top->type == T_LPAR || top->type == T_NOT || getPriority(top) < getPriority(*cur)){
						// push do zasobniku je na konci case
						break;
					}
				
					// pokud je na vrcholu operator s vyssi prioritou -> 
					// 		presunout na vystup a opakovat 3. krok
					if(getPriority(top) >= getPriority(*cur)){
						// odstranit z vrcholu
						top = (struct toc*)stackPop(stack);
						// vytvori ASTnode z tokenu
						makeAstFromToken(top, &aststack);
					}
										
					// na promennou za vrchol ulozi top prvek
					top = (struct toc*)stackTop(stack);		
				}	
				// ulozit aktualni prvek na vrchol zasobniku
				stackPush(stack, (*cur));	
				break;
			default:
				// Konec vyrazu - token ktery nespada do zadne kategorie
				if(!stackEmpty(stack)){
					// v pripade, ze ve stacku jsou data
					now = (struct toc*)stackPop(stack);

					// prebytek zavorek
					if(now->type == T_LPAR || now->type == T_RPAR){
						E("Syntax error - too many parentheses");
						exit(synt);
					}					
					
					while(now){
						// popnuto uz bylo, takze zpracovat
						makeAstFromToken(now, &aststack);						
						// popnout vrchol
						now = (struct toc*)stackPop(stack);
					}				
				}
				// v pripade, ze je v zasobniku jen jeden prvek
				// vratit ho, je to vysledek SY algoritmu
				if(aststack->Length != 1){
					E("expression: Shunting yard error - wrong expression syntax");
					exit(synt);
				}
			
				// vrati vrchol 
				D("expression: Returning top layer");
				return (struct astNode*)stackPop(aststack);
		}

		last = (*cur)->type;
		
		// get next token
		if(readNew){
			(*cur) = getToc();
		}
		else{
			readNew = true;
		}
	}
	
	E("expression: Syntax error");
	exit(synt);
}
