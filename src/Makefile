CC=gcc
CFLAGS=-std=c99 -g -Wall -pedantic -Wextra

all: 
	$(CC) $(CFLAGS) Ast.c main.c Scanner.c GC.c Log.c String.c -o test 

clean:
	rm -fv *.o *.gch
