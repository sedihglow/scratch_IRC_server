BIN = ./bin/serv_irc.out
CC=gcc
CCFLAGS = -I ./include -I ../shared/include -c -Wall -Wextra -pthread 
CCFLAGS+= -m64 -O0 -g -pedantic -std=c99
LDFLAGS = -pthread -m64

SRCS=$(wildcard ./src/*.c) $(wildcard ../shared/src/*.c)
OBJS=$(SRCS:.c=.o)

.PHONY: all
all: $(BIN)

$(BIN):$(OBJS)
						$(CC) $(OBJS) -o $@ $(LDFLAGS)
						
%.o:%.c
						$(CC) $(CCFLAGS) -o $@ -c $<

.PHONY: valgrind 
valgrind:				# check for memory leak
						$(info -- Checking For Memory Leaks --)
						make
						valgrind --leak-check=full ./bin/serv_irc.out

.PHONY: debug
debug:					# GNU debugger
						$(info -- Debugging --)
						gdb ./bin/serv_irc.out

.PHONY: clean
clean:					# clean the directory
						$(info -- Cleaning The Directory --)
						rm -rf ./src/*.o ../shared/src/*.o ./bin/serv_irc.out
.PHONY: run
run:					# run the program as follows
						$(info -- Running Program --)
						make
						./bin/serv_irc.out
