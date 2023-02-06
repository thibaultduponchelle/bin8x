
.PHONE: all clean test

all: bin8x

bin8x: bin8x.c
	gcc -o bin8x bin8x.c -Wall -pedantic -std=c99


test: all
	./test/bats/bin/bats test

clean:
	rm -f bin8x *.83p *.82p *.8xp __temp__.bin
	
