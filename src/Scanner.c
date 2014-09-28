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
	struct string str;

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
		
			if(isdigit(c)
		
		
		case KA_ERR:
		case default:
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
			while(c != '}' || != EOF )
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


