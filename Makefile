CC=gcc

CFLAGS=-Wall -g -lpthread

SRC=$(wildcard *.c)
BIN=$(SRC:%.c=%)

all: $(BIN)
				@echo "Done"

clean:
				rm -f server
				rm -f client
				rm -rf *.dSYM
