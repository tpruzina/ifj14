/******************************************************************************
 *
 * TODO: Project Title
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

FILE *fd;

/* get_toc - cita zo suboru dalsi token
 * @vstup:	otvoreny file descriptor
 * @vystup:	vraci kod tokenu (vid scanner.h)
 * 		v (struct toc *) vrati dynamicky alokovanu strukturu s datami
 */
struct toc *
get_toc()
{
	int c;		// nacitany aktualny znak
	int state;	// aktualny stav
	struct toc *toc;
//	struct string str;
	
	toc_init(&toc);
	ASSERT(toc);
	
	// fd = global->fd;
	state = KA_START;

	while (true)
	{
		// todo: kontrola ci toto je validna, specialne pri vyrazoch
		// apod
		c = tolower(fgetc(fd));

		switch(state)
		{
		case KA_START:
			// preskocime commenty a whitespace
			skip_ws_and_comments();
			
			if(isalpha(c) || '_' == c) // asi identifikator
			{
				//add2str
				state = KA_IDENT;
			}
			else if(isdigit(c))
			{
				//add2str
				state = KA_INTEGER;
			}
			else if(EOF == c)
			{
				toc->type = T_EOF;
			}
			break;


		case KA_INTEGER:	//uz mame cislicu
			if(isdigit(c))
			{
				//add2str	// 123
			}
			else if ('.' == c)
			{
				//add2str
				state = KA_INT_DOT;	// 123.
			}
			else if('e' == c)
			{
				//add2str
				state = KA_REAL_EXP;	// 123e
			}
			else	//mame integer
			{
				ungetc(c,fd);
				
				toc->type = T_INTEGER;
				//toc->data.integer = atoi(str);
				//free string???
				return toc;
			}	
			break;

		case KA_INT_DOT:
			if(isdigit(c))	//mame bodku '.', musi byt cislo
			{
				//add2str
				state = KA_REAL;
			}
			else
				state = KA_ERR;
			break;

		case KA_REAL:		// uz bola .x
			if(isdigit(c))
			{
				//add2str
			}
			else if ('e' == c)
			{
				//add2str
				state = KA_REAL_EXP;
			}
			else
			{
				// success, mame real cislo!!
				ungetc(c,fd);
				toc->type = T_REAL;
				//toc->data.real = atof(str);
				return toc;
			}
			break;
			
		case KA_REAL_EXP:	// mame exponent, moze byt +,- a cislo
			if(isdigit(c))
			{
				//add2str
				state = KA_REAL_EXP;
			}
			else if ('-' == c || '+' == c)
			{
				//add2str
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
				//add2str
			}
			else	//mame real
			{
				ungetc(c,fd);
				toc->type = T_REAL;			
				//toc->data.real = atof(str);
				return toc;
			}
			break;

		case KA_IDENT: // mali sme [_,a-z]
			if (isalnum (c) || (c == '_'))
			{
				//add2str
			}
			else
			{
				ungetc(c,fd);
/*
				// klicove slova (zoradena dle zadani) 3.1
				// tu bude treba davat bacha na rozlisenie tokenov
				// na klucove slova vztahujuce sa na datove typy
				// T_KW_REAL vs T_REAL atp.

				if(!strcmp("begin",str))
					toc->type = T_BEGIN;
				else if(!strcmp("boolean",str))
					toc->type = T_KW_BOOLEAN;
				else if (!strcmp("do",str))
					toc->type = T_DO,
				else if (!strcmp("else",str))
					toc->type = T_ELSE;
				else if (!strcmp("end",str))
					toc->type = T_END;
				else if (!strcmp("false",str))
					toc->type = T_FALSE;
				else if (!strcmp("find",str))
					toc->type = T_FIND;
				else if (!strcmp("forward",str))
					toc->type = T_FORWARD;
				else if (!strcmp("function",str))
					toc->type = T_FUNCTION;
				else if (!strcmp("if",str))
					toc->type = T_IF;
				else if (!strcmp("integer",str))
					toc->type = T_KW_INTEGER;
				else if (!strcmp("readln",str))
					toc->type = T_READLN;
				else if (!strcmp("real",str))
					toc->type = T_KW_REAL;
				else if (!strcmp("sort",str))
					toc->type = T_SORT;
				else if (!strcmp("string",str))
					toc->type = T_STRING,
				else if (!strcmp("then",str))
					toc->type = T_THEN;
				else if (!strcmp("true",str))
					toc->type = T_TRUE;
				else if (!strcmp("var",str))
					toc->type = T_VAR;
				else if (!strcmp("while",str))
					toc->type = T_WHILE;
				else if (!strcmp("write",str))
					toc->type = T_WRITE;
				else if (!strcmp("",str))
					toc->type = T_
				else if (!strcmp("",str))
					toc->type = 
			else
				{
*/					toc->type = T_ID;
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
	ASSERT(fd);
	int c;

	while(c = fgetc(fd))
	{
		if(c == '{')
		{
			while(c != '}' || c != EOF )
				c = fgetc(fd);
		}
		else if(c != ' ')
			break;
	}
	ungetc(c,fd);
}


#ifdef _TEST
// cisty compile check
int main(int argc, char **argv)
{
	struct toc *tmp;
	
	if(argc != 1)
	{
		fprintf(stderr, "ocakavany jeden parameter - cesta k souboru\n");
		return -1;
	}
	fd = fopen(argv[1],"r");
	if(!fd)
	{
		fprintf(stderr, "nepodarilo sa otevrit soubor\n");
		return -1;
	}

	while (true)
	{
		tmp = get_toc();
		printf("%d\n", tmp->type);
		free(tmp);
		tmp = NULL;
	}
	return 0;
}

#endif


