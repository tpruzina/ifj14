/*
 * Log.c
 *
 * Obstarava kontrolni, chybove, ladici vypisy. Barevne odlisene od obycejnych vypisu.
 * Procedury zacinajici na Log pouzivaji pouze StdOut
 * Procedury zacinajici na Write pouzivaji StdErr
 * 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "Structs.h"
#include "Log.h"

/**
 * Pise chybovou zpravu, cervene
 */
void Log(char* msg, int type, int frm){
	if(type != ERROR)
		if(!PRINT) return;
	
	char* fcol;
	char from[64];
	char* mcol;
	
	switch(type){
		case DEBUG:
			if(!DBG)
				return;				
			
			mcol = COLOR_GRN;
			break;
		case ERROR:
			mcol = COLOR_RED;
			break;
		case WARNING:
			if(!WRG)
				return;
		
			mcol = COLOR_YEL;
			break;
	}
	
	switch(frm){
		case MAIN: {
			fcol = COLOR_LGRN;
			sprintf(from, "[  MAIN  ]");
			break;
		}
		case GC: {
			fcol = COLOR_LYEL;
			sprintf(from, "[   GC   ]");
			break;
		}
		case STRING: {
			fcol = COLOR_LCYN;
			sprintf(from, "[ STRING ]");
			break;
		}
		case STACK: {
			fcol = COLOR_LBLU;
			sprintf(from, "[ STKQUE ]");
			break;
		}
		case AST: {
			fcol = COLOR_LRED;
			sprintf(from, "[   AST  ]");
			break;
		}
		case IAL: {
			fcol = COLOR_DGRY;
			sprintf(from, "[   IAL  ]");
			break;
		}
		case PARSER: {
			fcol = COLOR_LMGN;
			sprintf(from, "[ PARSER ]");
			break;
		}
	}
	
	if(type == ERROR)	
		fprintf(stderr, " %s\t %s \t %d\n", from, msg,  global.lineCounter);
	else
		fprintf(stdout, " %s\t %s\n", from, msg);
}
