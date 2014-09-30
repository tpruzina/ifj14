#ifndef STRUCTS_H
#define STRUCTS_H

#include "String.h"
#include <stdbool.h>

/**
 * Konstanty
 */
#define True 1
#define False 0

/**
 * Slouzi pro urceni typu tokenu, ktery lex. analyzator nacte
 */
enum tokenType {
	// klicove slova
	T_START,	// program
	T_BEGIN,	// begin
	T_END,  	// end
	T_FUNC,		// function
	T_FORW,		// forward
	T_WHILE, 	// while
	T_RPT,		// repeat
	T_UNTIL,	// until
	T_IF,		// if
	T_ELSE,		// else
	T_THEN,		// then
	T_VAR,		// var
	T_ID,		// cokoliv odpovidajici identifikatoru
	T_TEXT,		// cokoliv odpovidajici stringu
	T_NMB,		// cokoliv odpovidajici cislu
	// datove typy
	T_INT,		// integer
	T_REAL,		// real
	T_BOOL,		// boolean
	T_STR,		// string
	T_ARR,		// array
	// obecne
	T_OF,		// of
	T_TRUE,		// true
	T_FALSE,	// false
	// inline
	T_FIND,		// find
	T_SORT,		// sort
	T_RDLN,		// readln
	T_WRT,		// write
	// aritmetika a logika
	T_ASGN,		// :=
	T_EQV,   	// =
	T_NEQV, 	// <>
	T_GRT,		// >
	T_LSS,		// <
	T_GEQV,		// >=
	T_LEQV,		// <=
	T_ADD,		// +
	T_SUB,		// -
	T_MUL,		// *
	T_DIV,		// /
	T_MOD,		// mod
	T_AND,		// and
	T_OR,		// or
	T_NOT,		// not
	// specialni znaky
	T_SCOL,		// ;
	T_COL,		// :
	T_DOT,		// .
	T_COM,		// ,
	T_APS,		// '
	T_LPAR,		// (
	T_RPAR,		// )
	T_LBRC,		// [
	T_RBRC,		// ]
	T_LCBR,		// {
	T_RCBR,		// }
	T_DDOT,		// ..
	T_USC		// _
};

/*
char* keywords[37] = {
	"program\0",
	"begin\0",
	"end\0",
	"procedure\0",
	"function\0",
	"while\0",
	"for\0",
	"to\0",
	"downto\0",
	"repeat\0",
	"until\0",
	"break\0",
	"continue\0",
	"case\0",
	"if\0",
	"else\0",
	"then\0",
	"var\0",
	"const\0",
	"integer\0",
	"real\0",
	"boolean\0",
	"char\0",
	"type\0",
	"set\0",
	"in\0",
	"record\0",
	"array\0",
	"file\0",
	"of\0",
	"do\0",
	"nil\0",
	"div\0",
	"mod\0",
	"and\0",
	"or\0",
	"not\0"
};
*/

/**
 * Struktura tokenu, pro uchovani typu a obsahu
 */
struct token {
	enum tokenType type;
	struct String* value;
};

/**
 * Zastupuje zaznam v seznamu alokovanych prostredku
 */
struct gcItem
{
	void* ptr;
	struct gcItem* next;
};

/**
 * Garbage collector ktery v sobe uchovava pocet alokovanych prostredku
 */
struct gc
{
	struct gcItem* list;
	int listLength;
};

/**
 * Chybove kody
 */
enum errno { ok = 0, 
	lex = 1, 		// chybna struktura aktualniho lexemu
	synt = 2, 		// chybna syntaxe struktury programu
	sem_prog = 3, 	// nedefinovana promenna/fukce, pokus o redefinici funkce/promenne
	sem_komp = 4, 
	sem_else = 5, 
	run_num = 6, 
	run_ninit = 7, 
	run_div = 8, 
	run_else = 9, 
	intern = 99 };

/**
 * Vsechny hlavni soucasti.
 * Hlavni struktura programu
 */
struct mainAll {
	struct gc* gc;
	FILE* src;
	enum errno errno;
};

/**
 * Potrebne typy uzlu do AST
 */
enum astNodeType {
	AST_NONE,
	AST_START,
	AST_END,
	AST_FUNC,
	AST_FORW,
	AST_IF,
	AST_WHILE,
	AST_REPEAT,
	AST_ASGN,
	AST_EQV,
	AST_NEQV,
	AST_GRT,
	AST_LSS,
	AST_GEQV,
	AST_LEQV,
	AST_ADD,
	AST_SUB,
	AST_MUL,
	AST_DIV,
	AST_AND,
	AST_OR,
	AST_XOR,
	AST_NOT,
	AST_NUM,
	AST_STR,
	AST_ID
};

/**
 * Jeden uzel AST
 */
struct astNode {
	enum astNodeType type;
	
	struct astNode* left;
	struct astNode* right;
	
	void* value;
};

/**
 * Uzel tabulky symbolu
 */
struct symbolTableNode {
	struct String* name;
	union {
		struct String* str_data;
		int int_data;
		double real_data;
		bool bool_data;
	} data;
	
	struct symbolTableNode* left;
	struct symbolTableNode* right;	
};


#endif
