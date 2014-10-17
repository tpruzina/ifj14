/******************************************************************************
 * %FILE%  [:VIM_EVAL:]ls[:END_EVAL:]
 * Author: Tomas Pruzina <pruzinat@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

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

