/*
 * Log.h
 *
 * Hlavickovy soubor knihovny Log.
 * Obsahuje predevsim potrebne definice barev 
 * 
 */

/*
 * Definice barev
 */
#ifndef _LOGH_
#define _LOGH_

#ifdef _COLOR
#define COLOR_NRM  "\x1B[0m"
#define COLOR_RED  "\x1B[31m"
#define COLOR_GRN  "\x1B[32m"
#define COLOR_YEL  "\x1B[33m"
#define COLOR_BLU  "\x1B[34m"
#define COLOR_MAG  "\x1B[35m"
#define COLOR_CYN  "\x1B[36m"
#define COLOR_WHT  "\x1B[37m"
#define COLOR_DGRY "\033[01;30m"
#define COLOR_LRED "\033[01;31m"
#define COLOR_LGRN "\033[01;32m"
#define COLOR_LYEL "\033[01;33m"
#define COLOR_LBLU "\033[01;34m"
#define COLOR_LMGN "\033[01;35m"
#define COLOR_LCYN "\033[01;36m"
#endif

#ifndef _COLOR
#define COLOR_NRM  ""
#define COLOR_RED  ""
#define COLOR_GRN  ""
#define COLOR_YEL  ""
#define COLOR_BLU  ""
#define COLOR_MAG  ""
#define COLOR_CYN  ""
#define COLOR_WHT  ""
#define COLOR_DGRY ""
#define COLOR_LRED ""
#define COLOR_LGRN ""
#define COLOR_LYEL ""
#define COLOR_LBLU ""
#define COLOR_LMGN ""
#define COLOR_LCYN ""
#endif

#ifdef _DEBUG
#define PRINT 1
#define DBG 1
#define WRG 1
#define PRT 1
#endif

#ifndef _DEBUG
#define PRINT 0
#define DBG 0
#define WRG 0
#define PRT 0
#endif


enum LogFrom { MAIN, GC, STRING, STACK, AST, IAL, PARSER , SYMTABLE};
enum LogType { DEBUG, ERROR, WARNING };
/*
 * Definice funkci knihovny Log
 */
void Log(char*, int, int);

#endif
