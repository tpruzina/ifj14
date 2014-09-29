CFLAGS=-g -std=c99

all: 
	/bin/true

test: test_build_scanner

test_build_scanner: Scanner.c String.c GC.c
	$(CC) $(CFLAGS) -D_DEBUG -D_TEST Scanner.c -o test_scanner
	rm test_scanner
clean:
	/bin/true

