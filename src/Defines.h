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

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <stdbool.h>	// boolean [true,false]
#include <stdlib.h>	// todo: delete : malloc
#include <stdio.h>	
#include <assert.h>

#ifdef _DEBUG
// (nekompilovat ak nie je definovany _DEBUG, prepinac "gcc -D_DEBUG")
#define ASSERT(expr) do {										\
	if(!(expr)){												\
	fprintf(stderr,"Skapal som v %s:%d\n",__FILE__,__LINE__);	\
	exit(intern);												\
	}															\
} while (0)

#endif // _DEBUG


#ifndef _DEBUG	//!(_DEBUG)
#define ASSERT(X) { } 
#endif	// !(_DEBUG)


#endif /* __DEFINES_H__ */

