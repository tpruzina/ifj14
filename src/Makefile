CFLAGS=-g

all: 
	/bin/true

test: test_build_scanner

test_build_scanner: Scanner.c
	$(CC) $(CFLAGS) -D_DEBUG -D_TEST $< -o test_scanner
	rm test_scanner
clean:
	/bin/true

