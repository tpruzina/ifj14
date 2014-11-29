#!/bin/bash

if [ $#	-ne 1 ]; then
	echo "supply login name as parameter" && exit 1
else
	echo "########## building && testing"
	make || exit 1

	echo "########## creating ${1}.tar.gz"
	#tar uklada veci vcetne cesty takze si vytvorime .tmp adresar
	mkdir -p .tmp && cd .tmp || exit 1
	cp \
		../src/Makefile		\
		../src/Ast.c		\
		../src/Ast.h		\
		../src/Constants.h	\
		../src/Defines.h	\
		../src/GC.c		\
		../src/GC.h		\
		../src/ial.c		\
		../src/ial.h		\
		../src/Interpret.c	\
		../src/Interpret.h	\
		../src/Log.c		\
		../src/Log.h		\
		../src/main.c		\
		../src/main_interpret.c	\
		../src/Parser.c		\
		../src/Parser.h		\
		../src/Scanner.c	\
		../src/Scanner.h	\
		../src/Stack.c		\
		../src/Stack.h		\
		../src/String.c		\
		../src/String.h		\
		../src/Structs.h	\
		../src/SymbolTable.c	\
	. || exit 1
	
	##pridaj test build po nakopirovani zdrojakov???????
	
	tar cvfz ../${1}.tar.gz * || exit 1
	cd .. && rm -r .tmp
fi
