MAIN=main
GCC=gcc
CFLAGS=-std=c99 -g -Wall -pedantic -Wextra

all:
	$(GCC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c ial.c Scanner.c Parser.c $(MAIN).c -o $(MAIN)
	
clean:
	rm -fv *.o *.gch
