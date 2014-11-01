CFLAGS=-std=c99 -g -Wall -pedantic -Wextra -Wno-switch
SOURCES=ial.c GC.c Log.c String.c Stack.c Ast.c Scanner.c Parser.c

ALL: all
all: interpret main

interpret: $(SOURCES) Interpret.c main_interpret.c *.h
	$(CC) $(CFLAGS) -U_DEBUG GC.c ial.c Log.c String.c Stack.c Ast.c Scanner.c Parser.c Interpret.c main_interpret.c -o interpret

main: $(SOURCES) main.c
	$(CC) $(CFLAGS) -D_DEBUG -D_COLOR ial.c GC.c Log.c String.c Stack.c Ast.c Scanner.c Parser.c main.c -o main

test: clang cppcheck

clang:
	env scan-build --use-analyzer=/usr/bin/clang++ make

cppcheck:
	env cppcheck --enable=all --std=c99 --language=c *.c

clean:
	rm -fv *.o *.gch main test interpret
