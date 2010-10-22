CFLAGS = -O2 -Wall

.PHONY: test clean

all: testsuite

testsuite: testsuite.c netstring_stream.o

test: testsuite
	./testsuite

clean:
	rm -f *.o testsuite