CC=cc
CFLAGS=-Wall -Wextra -Werror -pedantic -ggdb

.PHONY: all
all: petals.o
	$(CC) $(CFLAGS) -o petals petals.o

petals.o: main.c
	$(CC) $(CFLAGS) -c main.c -o petals.o

clean:
	rm petals.o petals
