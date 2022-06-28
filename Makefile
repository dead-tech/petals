CC=cc
CFLAGS=-Wall -Wextra -Werror -pedantic -ggdb

.PHONY: all
all: main.o utils.o tokenizer.o interpreter.o
	$(CC) $(CFLAGS) -o petals main.o utils.o tokenizer.o interpreter.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

tokenizer.o: src/tokenizer/tokenizer.c
	$(CC) $(CFLAGS) -c src/tokenizer/tokenizer.c -o tokenizer.o

interpreter.o: src/interpreter/interpreter.c
	$(CC) $(CFLAGS) -c src/interpreter/interpreter.c -o interpreter.o

utils.o: src/utils/utils.c
	$(CC) $(CFLAGS) -c src/utils/utils.c -o utils.o

clean:
	rm main.o tokenizer.o utils.o petals
