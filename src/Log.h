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

/*
 * Definice funkci knihovny Log
 */
void LogError(char*);
void WriteError(char*);

void LogWarning(char*);
void WriteWarning(char*);

void LogDebug(char*);
void WriteDebug(char*);
