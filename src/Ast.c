#include <stdio.h>
#include <stdlib.h>
#include "GC.h"
#include "Structs.h"
#include "Log.h"

struct astNode* makeNewAST(){
	struct astNode* ast = (struct astNode*)gcMalloc(sizeof(struct astNode));
	if(ast == NULL){
		Log("New ast not allocated", ERROR, AST);
		global.errno = intern;
		return NULL;
	}
	ast->data.str = NULL;
	ast->data.integer = 0;
	ast->data.real = 0;
	ast->data.boolean = False;
	
	ast->left = NULL;
	ast->right = NULL;
	ast->type = AST_NONE;
	
	return ast;
}

void prtAst(struct astNode* nd, int lvl, char** str){
	if(nd->right != NULL)
		prtAst(nd->right, lvl+1, str);
		
	for(int i = 0; i < lvl; i++) fprintf(stderr, "  ");
	switch(nd->type){
		case AST_NONE:
		case AST_START:
		case AST_CMD:
			fprintf(stderr, "%s%s%s\n", COLOR_LGRN, str[nd->type], COLOR_NRM);
			break;	
		case AST_INT:
			fprintf(stderr, "%s%s\t[ %d ]%s\n", COLOR_LYEL, str[nd->type], nd->data.integer, COLOR_NRM);
			break;
		case AST_REAL:
			fprintf(stderr, "%s%s\t[ %g ]%s\n", COLOR_LYEL, str[nd->type], nd->data.real, COLOR_NRM);
			break;
		case AST_BOOL:
			fprintf(stderr, "%s%s\t[ %s ]%s \n", COLOR_LYEL, str[nd->type], (nd->data.boolean)?"TRUE":"FALSE", COLOR_NRM);
			break;
		case AST_STR:
			fprintf(stderr, "%s%s\t[ \"%s\" ]%s\n", COLOR_LYEL, str[nd->type], (nd->data.str == NULL)?"<NULL>":nd->data.str->Value, COLOR_NRM);
			break;
		case AST_ARR:
		case AST_ID:
			fprintf(stderr, "%s%s\t[ %s ]%s\n", COLOR_LYEL, str[nd->type], nd->data.str->Value, COLOR_NRM);
			break;
		case AST_IF:
		case AST_WHILE:
		case AST_REPEAT:
		case AST_ASGN:
			fprintf(stderr, "%s%s%s\n", COLOR_LBLU, str[nd->type], COLOR_NRM);
			break;
		case AST_FUNC:
		case AST_CALL: {
			struct String* name = (struct String*)nd->other;
			fprintf(stderr, "%s%s\t[ %s ]%s\n", COLOR_LCYN, str[nd->type], name->Value, COLOR_NRM);
			break;
		}
		case AST_EQV:
		case AST_NEQV:
		case AST_GRT:
		case AST_LSS:
		case AST_GEQV:
		case AST_LEQV:
		case AST_ADD:
		case AST_SUB:
		case AST_MUL:
		case AST_DIV:
		case AST_AND:
		case AST_OR:
		case AST_XOR:
		case AST_NOT:
			fprintf(stderr, "%s%s%s\n", COLOR_LMGN, str[nd->type], COLOR_NRM);
			break;			
		default:
			fprintf(stderr, "%s\n", str[nd->type]);
			break;
	}
			
	if(nd->type == AST_IF || nd->type == AST_WHILE || nd->type == AST_REPEAT){
		if(nd->other != NULL){
			fprintf(stderr, "%s", COLOR_GRN);
			prtAst(nd->other, lvl + 3, str);
			fprintf(stderr, "%s", COLOR_NRM);
		}
	}
	
	if(nd->left != NULL)
		prtAst(nd->left, lvl+1, str);
}
void printAst(struct astNode* ast){
	if(!PRT) return;

	if(ast == NULL){
		Log("printAst: ast == null", ERROR, AST);
		return;
	}
	
	char* nodes[] = {
		"AST_NON",
		"AST_STT",
		"AST_END",
		"AST_FNC",
		"AST_CAL",
		"AST_IF",
		"AST_WHL",
		"AST_RPT",
		"AST_ASG",
		"AST_EQV",
		"AST_NEQ",
		"AST_GRT",
		"AST_LSS",
		"AST_GEQ",
		"AST_LEQ",
		"AST_ADD",
		"AST_SUB",
		"AST_MUL",
		"AST_DIV",
		"AST_AND",
		"AST_OR",
		"AST_XOR",
		"AST_NOT",
		"AST_NUM",
		"AST_ID",
		"AST_INT",
		"AST_REL",
		"AST_BOL",
		"AST_STR",
		"AST_ARR",
		"AST_CMD",
		"AST_FOR",
		"AST_FOR_TO",
		"AST_FOR_DOWNTO",
		"AST_SWITCH",
		"AST_WRITE",
		"AST_READLN",
		"AST_COPY",
		"AST_LENGTH",
		"AST_FIND",
		"AST_SORT",
	};
	
	prtAst(ast, 0, nodes);
}
