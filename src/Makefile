MAIN=main
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra
SOURCES_SCANNER=

<<<<<<< HEAD
all: tonda scanner


tonda:
	$(GCC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)
=======

all: test_tonda test_scanner

test_tonda:
	$(CC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c $(MAIN).c -o $(MAIN)
	
test_scanner:
	$(CC) $(CFLAGS) -o test_scanner

clean:
	rm -fv *.o *.gch main
>>>>>>> master

scanner:
	$(CC) $(CFLAGS) GC.c String.c Scanner.c -D_TEST

clean:
	rm -fv *.o *.gch
