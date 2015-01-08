#!/bin/bash

if [ $#	-ne 1 ]; then
	echo "supply login name as parameter" && exit 1
else
	make || exit 1

	echo "########## creating ${1}.zip"
	#tar uklada veci vcetne cesty takze si vytvorime .tmp adresar
	mkdir -p .tmp && cd .tmp || exit 1
	cp \
		../rozdeleni		\
		../rozsireni		\
		../src/Makefile		\
		../src/ast.c		\
		../src/ast.h		\
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
		../docs/dokumentace/dokumentace.pdf \
	. || exit 1
	
	echo "########## building && testing #########"
	make || exit 1
	
	make clean || exit 1

	echo "########## build successful ############"

	echo "########## creating archive ############"
	
	zip ../${1}.zip * || exit 1
	cd .. && rm -r .tmp

	echo "########## created ################"
	

	echo "######### running is_it_ok.sh #####"
	bash scripts/is_it_ok.sh ${1}.zip .tmp2 || exit 1
	rm -rf .tmp2

	echo "######### is_it_ok DONE ###########"
fi
