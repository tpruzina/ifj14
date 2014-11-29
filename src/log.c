/*****************************************************************************
****                                                                      ****
****    PROJEKT DO PREDMETU IFJ                                           ****
****                                                                      ****
****    Nazev:     Implementace interpretu imperativního jazyka IFJ14     ****
****    Datum:                                                            ****
****    Autori:    Marko Antonín    <xmarko07@stud.fit.vutbr.cz>          ****
****               Pružina Tomáš    <xpruzi01@stud.fit.vutbr.cz>          ****
****               Kubíček Martin   <xkubic34@stud.fit.vutbr.cz           ****
****               Juřík Martin     <xjurik08@stud.fit.vutbr.cz           ****
****               David Petr       <xdavid15@stud.fit.vutbr.cz>          ****
****                                                                      ****
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "log.h"

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
		fprintf(stderr, " %s%s\t%s %s%s%s \t %u\n", fcol, from, COLOR_NRM, mcol, msg, COLOR_NRM, global.lineCounter);
	else
		fprintf(stdout, " %s\t %s\n", from, msg);
}
