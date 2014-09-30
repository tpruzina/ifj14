/******************************************************************************
 *
 * Author: Tomas Pruzina <pruzinat@gmail.com>
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 ******************************************************************************/

#include <ctype.h> 	// tolower()
#include "Scanner.h"

#include "String.h"

/* get_toc - cita zo suboru dalsi token
 * @vstup:	otvoreny file descriptor
 * @vystup:	vraci alokovany token
 */
struct toc *
get_toc()
{
	int c;		// nacitany aktualny znak
	int state;	// aktualny stav
	struct toc *toc;
	struct String *str;
		
	toc_init(&toc);
	ASSERT(toc);
	ASSERT(global.src);

	state = KA_START;

	str = makeNewString();
	ASSERT(str);

	while (true)
	{
		// todo: kontrola ci toto je validna, specialne pri vyrazoch
		// apod
		c = tolower(fgetc(global.src));

		switch(state)
		{
		case KA_START:
			// preskocime commenty a whitespace
			skip_ws_and_comments();
			
			if(isalpha(c) || '_' == c) // asi identifikator
			{
				addChar(str,c);
				state = KA_IDENT;
			}
			else if(isdigit(c))
			{
				addChar(str,c);
				state = KA_INTEGER;
			}
			else if(EOF == c)
			{
				toc->type = T_EOF;
				return toc;
			}
			break;


		case KA_INTEGER:	//uz mame cislicu
			if(isdigit(c))
			{
				addChar(str,c);	// 123
			}
			else if ('.' == c)
			{
				addChar(str,c);
				state = KA_INT_DOT;	// 123.
			}
			else if('e' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP;	// 123e
			}
			else	//mame integer
			{
				ungetc(c,global.src);
				
				toc->type = T_INT;
				//toc->data.integer = atoi(str);
				//free string???
				return toc;
			}	
			break;

		case KA_INT_DOT:
			if(isdigit(c))	//mame bodku '.', musi byt cislo
			{
				addChar(str,c);
				state = KA_REAL;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL:		// uz bola .x
			if(isdigit(c))
			{
				addChar(str,c);
			}
			else if ('e' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP;
			}
			else
			{
				// success, mame real cislo!!
				ungetc(c,global.src);
				toc->type = T_REAL;
				//toc->data.real = atof(str);
				return toc;
			}
			break;
			
		case KA_REAL_EXP:	// mame exponent, moze byt +,- a cislo
			if(isdigit(c))
			{
				addChar(str,c);
				state = KA_REAL_EXP;
			}
			else if ('-' == c || '+' == c)
			{
				addChar(str,c);
				state = KA_REAL_EXP_PM;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL_EXP_PM:	// mali sme +-, dalej je povinne cislo
			if(isdigit(c))
			{
				// add2str
				state = KA_REAL_EXP_NUM;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL_EXP_NUM:	// mame real,exp,cislo, uz mozu byt len cisla	
			if(isdigit(c))
			{
				addChar(str,c);
			}
			else	//mame real
			{
				ungetc(c,global.src);
				toc->type = T_REAL;			
				//toc->data.real = atof(str);
				return toc;
			}
			break;

		case KA_IDENT: // mali sme [_,a-z]
			if (isalnum (c) || (c == '_'))
			{
				addChar(str,c);
			}
			else
			{
				ungetc(c,global.src);

				// klicove slova (zoradena dle zadani) 3.1
				// tu bude treba davat bacha na rozlisenie tokenov
				// na klucove slova vztahujuce sa na datove typy
				// T_KW_REAL vs T_REAL atp.

				if(!compareString(str,"begin"))
					toc->type = T_KW_BEGIN;
				else if(!compareString(str,"boolean"))
					toc->type = T_KW_BOOLEAN;
				else if (!compareString(str,"do"))
					toc->type = T_KW_DO;
				else if (!compareString(str,"else"))
					toc->type = T_KW_ELSE;
				else if (!compareString(str,"end"))
					toc->type = T_KW_END;
				else if (!compareString(str,"false"))
					toc->type = T_KW_FALSE;
				else if (!compareString(str,"find"))
					toc->type = T_KW_FIND;
				else if (!compareString(str,"forward"))
					toc->type = T_KW_FORW;
				else if (!compareString(str,"function"))
					toc->type = T_KW_FUNC;
				else if (!compareString(str,"if"))
					toc->type = T_KW_IF;
				else if (!compareString(str,"integer"))
					toc->type = T_KW_INT;
				else if (!compareString(str,"readln"))
					toc->type = T_KW_READLN;
				else if (!compareString(str,"real"))
					toc->type = T_KW_REAL;
				else if (!compareString(str,"sort"))
					toc->type = T_KW_SORT;
				else if (!compareString(str,"string"))
					toc->type = T_KW_STRING;
				else if (!compareString(str,"then"))
					toc->type = T_KW_THEN;
				else if (!compareString(str,"true"))
					toc->type = T_KW_TRUE;
				else if (!compareString(str,"var"))
					toc->type = T_KW_VAR;
				else if (!compareString(str,"while"))
					toc->type = T_KW_WHILE;
				else if (!compareString(str,"write"))
					toc->type = T_KW_WRT;
			else
				{
					toc->type = T_ID;
					// toto by malo byt v string.c ???
					//strcpy(toc->str,str);
					return toc;
				}
			}
		case KA_ERR:
		default:
			break;
		}
	}

	// chyba scanneru, mal by vyletiet inde!!
	exit(99);
	
	return toc;
}

void
toc_init(struct toc **toc)
{
	ASSERT(toc);

	struct toc *tmp = malloc(sizeof(struct toc));
	if(!tmp)
		exit(1);	// todo, error kod
	tmp->type = 0;		// todo: pridat define do headeru?
	
	*(toc) = tmp;
}

void skip_ws_and_comments()
{
	ASSERT(global.src);
	int c;

	while(EOF != (c = fgetc(global.src)))
	{
		if(c == '{')
		{
			while(c != '}' || c != EOF )
				c = fgetc(global.src);
		}
		else if(!isspace(c))
			break;
	}
	ungetc(c,global.src);
}





