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
	../docs/rozdeleni 			\
	../docs/dokumentace/dokumentace.pdf	\
	. || exit 1
	
	##pridaj test build po nakopirovani zdrojakov???????
	
	tar cvfz ../${1}.tar.gz * || exit 1
	cd .. && rm -r .tmp
fi
