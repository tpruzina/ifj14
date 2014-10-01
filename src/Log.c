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
#include "Log.h"

#define PRINT 1

/**
 * Pise chybovou zpravu, cervene
 */
void Log(char* msg, int type, int frm){
	if(!PRINT) return;
	
	char* fcol;
	char from[64];
	char* mcol;
	
	switch(frm){
		case MAIN: {
			fcol = COLOR_LGRN;
			sprintf(from, "MAIN");
			break;
		}
		case GC: {
			fcol = COLOR_LYEL;
			sprintf(from, "GC");
			break;
		}
		case STRING: {
			fcol = COLOR_LCYN;
			sprintf(from, "STRING");
			break;
		}
		case STACK: {
			fcol = COLOR_LBLU;
			sprintf(from, "STACK");
			break;
		}
		case AST: {
			fcol = COLOR_LRED;
			sprintf(from, "AST");
			break;
		}
		case SYMTABLE: {
			fcol = COLOR_DGRY;
			sprintf(from, "SYMTABLE");
			break;
		}
		case PARSER: {
			fcol = COLOR_LMGN;
			sprintf(from, "PARSER");
			break;
		}
	}
	
	switch(type){
		case DEBUG:
			mcol = COLOR_GRN;
			break;
		case ERROR:
			mcol = COLOR_RED;
			break;
		case WARNING:
			mcol = COLOR_YEL;
			break;
	}
	
	
	fprintf(stderr, "%s%s\t%s %s%s%s\n", fcol, from, COLOR_NRM, mcol, msg, COLOR_NRM);
}
