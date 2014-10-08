MAIN=main
CFLAGS=-std=c99 -g -Wall -pedantic -Wextra

all: main interpret

main: *.[c,h]
	$(CC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c Scanner.c Parser.c $(MAIN).c -o $(MAIN)

interpret: *.[c,h]
	$(CC) $(CFLAGS) GC.c Log.c String.c Stack.c Ast.c SymbolTable.c Scanner.c Parser.c Interpret.c main_interpret.c -o interpret

clean:
	rm -fv *.o *.gch main test interpret
