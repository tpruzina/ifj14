#ifndef STRUCTS_H
#define STRUCTS_H

/*
	Slouzi pro urceni typu tokenu, ktery lex. analyzator nacte
 */
enum token_type {
	T_START,	// program
	// klicove slova
	T_BEGIN,	// begin	OK
	T_END,  	// end		OK
	T_PROC, 	// procedure 	??		
	T_FUNC,		// function	OK
	T_WHILE, 	// while
	T_FOR,		// for
	T_FOR_TO,	// to
	T_FOR_DWNTO,// downto
	T_REPEAT,	// repeat
	T_UNTIL,	// until
	T_BREAK,	// break
	T_CONTINUE,	// continue
	T_CASE,		// case
	T_IF,		// if
	T_ELSE,		// else
	T_THEN,		// then
	T_VAR,		// var
	T_IDENTIF,	// cokoliv odpovidajici identifikatoru
	T_STRING,	// cokoliv odpovidajici stringu
	T_NUMBER,	// cokoliv odpovidajici cislu
	T_BOOLEAN,	// klicove slovo
	T_CONST,	// const
	// klicove slova pre datove typy
	T_KW_BOOLEAN,
	T_KW_INTEGER,
	T_KW_REAL,
	
	// datove typy
	T_INT,		// integer
	T_REAL,		// real
	T_BOOL,		// boolean
	T_CHAR,		// char
	T_TYPE,		// type 
	T_SET,		// set
	T_IN,		// in
	T_REC,		// record
	T_ARR,		// array
	T_FILE,		// file
	// obecne
	T_OF,		// of
	T_DO,		// do
	T_NIL,		// nil
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
	T_RDIV,		// /
	T_IDIV,		// div
	T_MOD,		// mod
	T_AND,		// and
	T_OR,		// or
	T_NOT,		// not
	T_DRF,		// ^
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
	T_USC,		// _
	T_EOF		// end of file
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

/*
enum errno { lex = 1, synt = 2, sem_prog = 3, sem_komp = 4, sem_else = 5, run_num = 6, run_ninit = 7, run_div = 8, run_else = 9, intern = 99 };
*/

/*
	Vsechny hlavni soucasti 
 */

/*
struct main_all {
	struct gc* gc;
	FILE* src;
	enum errno errno;
};
*/
#endif
