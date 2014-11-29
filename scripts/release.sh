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
		../rozdeleni		\
		../src/Makefile		\
		../src/ast.c		\
		../src/ast.h		\
		../src/constants.h	\
		../src/defines.h	\
		../src/gc.c		\
		../src/gc.h		\
		../src/ial.c		\
		../src/ial.h		\
		../src/interpret.c	\
		../src/interpret.h	\
		../src/log.c		\
		../src/log.h		\
		../src/main_interpret.c	\
		../src/parser.c		\
		../src/parser.h		\
		../src/scanner.c	\
		../src/scanner.h	\
		../src/stack.c		\
		../src/stack.h		\
		../src/string.c		\
		../src/string.h		\
		../src/structs.h	\
		../src/symboltable.c	\
		../docs/dokumentace/dokumentace.pdf \
	. || exit 1
	
	##pridaj test build po nakopirovani zdrojakov???????
	
	tar cvfz ../${1}.tar.gz * || exit 1
	cd .. && rm -r .tmp

	bash scripts/is_it_ok.sh ${1}.tar.gz .tmp || exit 1
fi
