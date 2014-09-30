MAIN=main
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
SOURCES_SCANNER=

all: tonda scanner

tonda:
	$(CC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)

scanner:
	$(CC) $(CFLAGS) GC.c String.c Scanner.c Log.c -D_TEST

clean:
	rm -fv *.o *.gch
