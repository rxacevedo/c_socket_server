CC=gcc
CFLAGS=-Wall -g 

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq ($(uname_S),Linux)
				CFLAGS += -pthread
endif

ifeq ($(uname_S),Darwin)
				CFLAGS += -lpthread
endif

SRC=$(wildcard *.c)
BIN=$(SRC:%.c=%)

all: $(BIN)
				@echo "Done"

clean:
				rm -f server
				rm -f client
				rm -rf *.dSYM
