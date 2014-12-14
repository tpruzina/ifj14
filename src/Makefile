CFLAGS=-std=c99 -g -O2 -march=native -Wall -pedantic -Wextra -Wno-switch
SOURCES=ial.c gc.c log.c string.c stack.c ast.c scanner.c parser.c

ALL: all
all: interpret

interpret: $(SOURCES) interpret.c main_interpret.c *.h
	$(CC) $(CFLAGS) -U_DEBUG -U_COLOR gc.c ial.c log.c string.c stack.c ast.c scanner.c parser.c interpret.c main_interpret.c -o ifj14

main: $(SOURCES) main.c
	$(CC) $(CFLAGS) -D_DEBUG -D_COLOR ial.c gc.c log.c string.c stack.c ast.c scanner.c parser.c main.c -o main

debug:	$(SOURCES) interpret.c main_interpret.c *.h
	$(CC) $(CFLAGS) -D_DEBUG -D_COLOR gc.c ial.c log.c string.c stack.c ast.c scanner.c parser.c interpret.c main_interpret.c -o ifj14

test: clang cppcheck

clang:
	env scan-build --use-analyzer=/usr/bin/clang++ make

cppcheck:
	env cppcheck --enable=all --std=c99 --language=c *.c

clean:
	rm -fv *.o *.gch main test interpret ifj14
