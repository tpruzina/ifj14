MAIN=main
CFLAGS=-std=c99 -g -Wall -pedantic -Wextra

all: interpret color main

main: *.[c,h]
	$(CC) $(CFLAGS) -D_DEBUG -D_COLOR GC.c Log.c ial.c String.c Stack.c Ast.c Scanner.c Parser.c $(MAIN).c -o $(MAIN)

interpret: *.[c,h]
	$(CC) $(CFLAGS) GC.c ial.c Log.c String.c Stack.c Ast.c Scanner.c Parser.c Interpret.c main_interpret.c -o interpret

color: *.[c,h]
	$(CC) $(CFLAGS) -D_DEBUG -D_COLOR ial.c GC.c Log.c String.c Stack.c Ast.c Scanner.c Parser.c Interpret.c main_interpret.c -o main

clean:
	rm -fv *.o *.gch main test interpret
