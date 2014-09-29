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
			{
				//mame integer
				ungetc(c,fd);
				//parse
				return T_NUMBER;
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

		case KA_IDENT: // mali sme [_,a-z]
			if (isalnum (c) || (c == '_'))
			{
				//add2str
			}
			else
			{
				ungetc(c,fd);

				/*
				*	SEM PRIDAT CHECKY NA VYNIMKY
				*	BEGIN
				*	END
				*	atp.
				*/

				
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


