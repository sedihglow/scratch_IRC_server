.PHONY: all
all:
	+$(MAKE) -C ./client all
	+$(MAKE) -C ./server all

.PHONY: client
client: 
	+$(MAKE) -C ./client all

.PHONY: server
server:
	+$(MAKE) -C ./server all

.PHONY: valgrind client
valgrind client:
	+$(MAKE) -C ./client valgrind

.PHONY: valgrind server 
valgrind server: 
	+$(MAKE) -C ./server valgrind

.PHONY: run client
run client:
	+$(MAKE) -C ./client run

.PHONY: run server
run server:
	+$(MAKE) -C ./server run

.PHONY: clean
clean:
	+$(MAKE) -C ./client clean
	+$(MAKE) -C ./server clean
