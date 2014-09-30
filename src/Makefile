MAIN=main
GCC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra

all: tonda scanner


tonda:
	$(GCC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)

scanner:
	$(CC) $(CFLAGS) GC.c String.c Scanner.c -D_TEST

clean:
	rm -fv *.o *.gch
