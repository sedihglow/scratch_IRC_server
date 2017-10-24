CC = gcc
CPPFLAGS = -c -Wall -Wextra -pthread -m64 -O0 -g -pedantic -std=c99 
LDFLAGS = -pthread -m64

cli_irc.out:			main.o err_handle.o convNum.o irc_client.o client.o
						$(CC) $(LDFLAGS) main.o irc_client.o err_handle.o convNum.o client.o -o cli_irc.out

main.o:					./main.c ./irc_client.h
						$(CC) $(CPPFLAGS) ./main.c

irc_client.o:			./irc_client.c ./client.h
						$(CC) $(CPPFLAGS) ./irc_client.c

client.o:				./client.c ../utility_sys.h
						$(CC) $(CPPFLAGS) ./client.c

err_handle.o:			../err_handle/err_handle.c
						$(CC) $(CPPFLAGS) ../err_handle/err_handle.c

convNum.o:				../convNum/convNum.c
						$(CC) $(CPPFLAGS) ../convNum/convNum.c

.PHONY: check
check:					# check for memory leak
						$(info -- Checking For Memory Leaks --)
						make
						valgrind --leak-check=full ./cli_irc.out

.PHONY: debug
debug:					# GNU debugger
						$(info -- Debugging --)
						gdb ./cli_irc.out

.PHONY: clean
clean:					# clean the directory
						$(info -- Cleaning The Directory --)
						rm -rf *.o cli_irc.out
.PHONY: run
all:					# run the program as follows
						$(info -- Running Program --)
						make
						./cli_irc.out
