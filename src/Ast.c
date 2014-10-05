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
	
	ast->left = NULL;
	ast->right = NULL;
	ast->type = AST_NONE;
	
	return ast;
}

void prtAst(struct astNode* nd, int lvl, char** str){
	if(nd->right != NULL)
		prtAst(nd->right, lvl+1, str);
		
	for(int i = 0; i < lvl; i++) fprintf(stderr, "   ");		
	fprintf(stderr, "%s\n", str[nd->type]);
	
	if(nd->left != NULL)
		prtAst(nd->left, lvl+1, str);
}
void printAst(struct astNode* ast){
	if(ast == NULL){
		Log("printAst: ast == null", ERROR, AST);
		return;
	}
	
	char* nodes[] = {
		"[ AST_NONE ]",
		"[ AST_START ]",
		"[ AST_END ]",
		"[ AST_FUNC ]",
		"[ AST_FORW ]",
		"[ AST_IF ]",
		"[ AST_WHILE ]",
		"[ AST_REPEAT ]",
		"[ AST_ASGN ]",
		"[ AST_EQV ]",
		"[ AST_NEQV ]",
		"[ AST_GRT ]",
		"[ AST_LSS ]",
		"[ AST_GEQV ]",
		"[ AST_LEQV ]",
		"[ AST_ADD ]",
		"[ AST_SUB ]",
		"[ AST_MUL ]",
		"[ AST_DIV ]",
		"[ AST_AND ]",
		"[ AST_OR ]",
		"[ AST_XOR ]",
		"[ AST_NOT ]",
		"[ AST_NUM ]",
		"[ AST_ID ]",
		"[ AST_INT ]",
		"[ AST_REAL ]",
		"[ AST_BOOL ]",
		"[ AST_STR ]"};
	
	prtAst(ast, 0, nodes);
}
