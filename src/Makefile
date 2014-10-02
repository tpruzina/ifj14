MAIN=main
GCC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra

all:
	$(GCC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c Scanner.c Parser.c $(MAIN).c -o $(MAIN)
	
clean:
	rm -fv *.o *.gch
