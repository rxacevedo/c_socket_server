CC=gcc

CFLAGS=-Wall -g 

ifeq ($(uname_S), Linux)
	CCFLAGS += -pthread
endif

ifeq ($(uname_S), Darwin)
	CCFLAGS += -lpthread
endif

SRC=$(wildcard *.c)
BIN=$(SRC:%.c=%)

all: $(BIN)
				@echo "Done"

clean:
				rm -f server
				rm -f client
				rm -rf *.dSYM
