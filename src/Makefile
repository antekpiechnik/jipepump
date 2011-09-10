CFLAGS=-I/usr/local/include/ -I. -Wall -pedantic -O3
LDFLAGS=-lcurl

all: pipejump

pipejump.o: pipejump.c
	$(CC) $(CFLAGS) -c -o $@ $^

pipejump: main.c pipejump.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o pipejump
