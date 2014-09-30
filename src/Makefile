MAIN=main
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
SOURCES_SCANNER=


all: test_tonda test_scanner

test_tonda:
	$(CC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)
	
test_scanner:
	$(CC) $(CFLAGS) -o test_scanner

clean:
	rm -fv *.o *.gch main

