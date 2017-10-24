CC = gcc
CPPFLAGS = -c -Wall -Wextra -pthread -m64 -O0 -g -pedantic -std=c99 
LDFLAGS = -pthread -m64

serv_irc.out:			main.o err_handle.o convNum.o irc_server.o server.o
						$(CC) $(LDFLAGS) main.o irc_server.o err_handle.o convNum.o server.o -o serv_irc.out

main.o:					./main.c ./irc_server.h
						$(CC) $(CPPFLAGS) ./main.c

irc_server.o:			./irc_server.c ./server.h
						$(CC) $(CPPFLAGS) ./irc_server.c

server.o:				./server.c ../utility_sys.h
						$(CC) $(CPPFLAGS) ./server.c

err_handle.o:			../err_handle/err_handle.c
						$(CC) $(CPPFLAGS) ../err_handle/err_handle.c

convNum.o:				../convNum/convNum.c
						$(CC) $(CPPFLAGS) ../convNum/convNum.c

.PHONY: check
check:					# check for memory leak
						$(info -- Checking For Memory Leaks --)
						make
						valgrind --leak-check=full ./serv_irc.out

.PHONY: debug
debug:					# GNU debugger
						$(info -- Debugging --)
						gdb ./serv_irc.out

.PHONY: clean
clean:					# clean the directory
						$(info -- Cleaning The Directory --)
						rm -rf *.o serv_irc.out
.PHONY: run
run:					# run the program as follows
						$(info -- Running Program --)
						make
						./serv_irc.out
