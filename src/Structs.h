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
	// klicove slova - zadani 3.1.1
	T_KW_BEGIN,	// begin
	T_KW_BOOLEAN,	// boolean
	T_KW_DO,	// do
	T_KW_ELSE,	// else
	T_KW_END,  	// end
	T_KW_ENDDOT,	// end.
	T_KW_FALSE,	// false
	T_KW_FIND,	// find
	T_KW_FORW,	// forward
	T_KW_FUNC,	// function
	T_KW_IF,	// if
	T_KW_INT,	// integer
	T_KW_READLN,	// readln
	T_KW_REAL,	// real
	T_KW_SORT,	// sort
	T_KW_STRING,	// string
	T_KW_THEN,	// then
	T_KW_TRUE,		// true
	T_KW_USES,		//uses
	T_KW_VAR,	// var
	T_KW_WHILE, 	// while
	T_KW_WRT,		// write
	T_KW_PROGRAM,
	T_KW_LENGTH,
	T_KW_COPY,
	T_KW_FOR,
	T_KW_CASE,
	// neni v zadani ako klucove slovo - ??? 
	T_KW_RPT,	// repeat
	T_UNTIL,	// until
	T_ID,	// cokoliv odpovidajici identifikatoru
	T_TEXT,	// cokoliv odpovidajici stringu
	T_NMB,	// cokoliv odpovidajici cislu
	// datove typy
	T_INT,
	T_REAL,		// real
	T_STR,		// string
	T_BOOL,
	T_ARR,		// array
	// obecne
	T_OF,		// of
	T_KW_SWITCH,
	T_KW_TO,
	T_KW_DOWNTO,
	// inline
	//T_RDLN,		// readln
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
	T_XOR,
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
	T_USC,		// _
	T_EOF		// EOF		
};


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
enum errno { 
	ok = 0, 
	lex = 1, 		// chybna struktura aktualniho lexemu
	synt = 2, 		// chybna syntaxe struktury programu
	sem_prog = 3, 	// nedefinovana promenna/fukce, pokus o redefinici funkce/promenne
	sem_komp = 4, //chyba typove kompatibility v aritmetickych, retezcovych a relacnich vyrazech, pripadne spatny pocet ci typ parametru u volani funkce
	sem_else = 5, // ostatni semanticke chyby
	run_num = 6, // nacitani ciselne hodnoty ze vstupu
	run_ninit = 7, // neinicializovana promenna
	run_div = 8, // deleni nulou
	run_else = 9, // ostatni behove chyby
	intern = 99 }; // alokace, chyba otvirani souboru, spatne parametry prikazove radky

/**
 * Vsechny hlavni soucasti.
 * Hlavni struktura programu
 */
struct mainAll {
	struct gc* gc;
	FILE* src;
	
	unsigned lineCounter;

	struct astNode* program;
	struct stack* symTable;
	struct symbolTableNode* funcTable;

	struct symbolTableNode* globalTable;

	int errno;
};
extern struct mainAll global;
/**
 * Potrebne typy uzlu do AST
 */
enum astNodeType {
	AST_NONE,
	AST_START,
	AST_END,
	AST_FUNC,
	AST_CALL,
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
	AST_ID, // promenna
	AST_INT, // literal
	AST_REAL, // literal
	AST_BOOL, // literal
	AST_STR, // literal
	AST_ARR,
	AST_CMD,
	AST_FOR,
	AST_FOR_TO,
	AST_FOR_DOWNTO,
	AST_SWITCH,
	AST_WRITE,
	AST_READLN,
	AST_COPY,
	AST_LENGTH,
	AST_FIND,
	AST_SORT
};

/**
 * Jeden uzel AST
 */
struct astNode {
	// AST_XXX ONLY!!!
	enum astNodeType type;
	
	struct astNode* left;
	struct astNode* right;
	
	void* other;

	// data type enum -- ONLY	
	int dataType;
	union {
		struct String* str;
		int integer;
		double real;
		bool boolean;	
	} data;
};

/**
 * Promenna pro definici funkci, obsahuje lokalni promenne a parametry
 * -------------------------------------------------------------------
 */
struct varspars {
	struct queue* vars;
	struct queue* pars;
};

enum dataType { DT_NONE, DT_INT, DT_REAL, DT_BOOL, DT_STR, DT_ARR };
/**
 * Uzel tabulky symbolu
 */
struct symbolTableNode {
	struct String* name;
	
	// data type enum --- ONLY
	int dataType;
	union {
		struct String* str_data;
		int int_data;
		double real_data;
		bool bool_data;
	} data;
	
	// pro odkaz na astNode s telem funkce
	void* other;
	
	struct symbolTableNode* left;
	struct symbolTableNode* right;	
};

struct dataTypeArray {
	int low;
	int high;
	int type;
	struct String* id;
};

// struktura potrebna na reverzny string z token typu
struct token2str{
	char	*str;
	int	type;
};


#endif
