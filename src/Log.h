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

#define PRINT 1
#define DBG 1
#define WRG 0
#define PRT 0

enum LogFrom { MAIN, GC, STRING, STACK, AST, IAL, PARSER };
enum LogType { DEBUG, ERROR, WARNING };
/*
 * Definice funkci knihovny Log
 */
void Log(char*, int, int);

