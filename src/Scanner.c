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

#include <ctype.h> // tolower()

#include "Scanner.h"
#include "String.h"

FILE *fd;

/* get_toc - cita zo suboru dalsi token
 * @vstup:	otvoreny file descriptor
 * @vystup:	vraci kod tokenu (vid scanner.h)
 * 		v (struct toc *) vrati dynamicky alokovanu strukturu s datami
 */
int
get_toc(struct toc **toc)
{
	int c;		// nacitany aktualny znak
	int state;	// aktualny stav
	struct toc *tmp;
//	struct string str;

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
			
			if(isalpha(c))
			{
				state = KA_STRING;
			}
			else if(isdigit(c))
			{
				//add2str
				state = KA_INTEGER;
			}
			break;


		case KA_INTEGER:
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
			else
				state = KA_ERR;
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

		case KA_REAL:		// uz bola '.'
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
				// parse ++ return
				return T_REAL;
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
			else
			{	// success
				return T_REAL;
			}
			break;

		case KA_STRING:
		case KA_ERR:
		default:
			break;
		}
	}
	return state;
}

void
token_init(struct toc **toc)
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
int main()
{
	return 0;
}

#endif


