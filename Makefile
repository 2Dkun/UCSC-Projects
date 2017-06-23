CFLAGS = -Wall -Wextra -Werror -pedantic
CC = gcc $(CFLAGS)

.PHONY: all
all	: encode decode

encode	: encode.o bv.o huffman.o queue.o stack.o
	$(CC) -o encode encode.o bv.o huffman.o queue.o stack.o

decode	: decode.o bv.o huffman.o stack.o
	$(CC) -o decode decode.o bv.o huffman.o stack.o

encode.o	: encode.c
	$(CC) -c encode.c

decode.o	: decode.c
	$(CC) -c decode.c

huffman.o	: huffman.c
	$(CC) -c huffman.c

bv.o	: bv.c
	$(CC) -c bv.c

queue.o	: queue.c
	$(CC) -c queue.c

stack.o	: stack.c
	$(CC) -c stack.c

.PHONY: clean
clean	:
	rm -f encode decode encode.o decode.o bv.o huffman.o queue.o stack.o
