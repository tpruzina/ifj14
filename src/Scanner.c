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

/* get_toc - cita zo suboru dalsi token
 * @vstup:	otvoreny file descriptor
 * @vystup:	vraci kod tokenu (vid scanner.h)
 * 		v (struct toc *) vrati dynamicky alokovanu strukturu s datami
 */
int
get_toc(struct toc *toc)
{
	int c;		// nacitany aktualny znak
	int state;	// aktualny stav
	struct toc *toc;



	switch(state)

}

static inline void
token_init(struct toc **toc)
{
	ASSERT(toc);
	(*toc) = malloc(sizeof(struct toc));
	//if(!*toc)
	
	(*toc)->type = 0;	//todo: pridat define do headeru?
	
}


