SOURCES=main.c lex.c parser.c interpret.c
CC=gcc
CFLAGS=-std=c99
LDFLAGS=
DEBUGFLAGS=-D_DEBUG -Wextra -Wall -pedantic-errors
TESTFLAGS=-D_TEST

ALL: all
all: ifj


ifj: $(SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o ../ifj2014

test:
	/bin/true

clean:
	/bin/true
