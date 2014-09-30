CC=gcc
CFLAGS=-std=c99 -g -Wall -pedantic -Wextra

all: test_tonda test_scanner

test_tonda:
	$(CC) $(CFLAGS) Scanner.c GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)

test_scanner:
	$(CC) $(CFLAGS) Scanner.c GC.c Log.c String.c 

clean:
	rm -fv *.o *.gch
