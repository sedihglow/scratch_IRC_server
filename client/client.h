/*******************************************************************************
 * Client side operation functinos and definitions.
 *
 * Filename: client.h
 * Written by: James Ross
 ******************************************************************************/
#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../utility_sys.h" /* useful macros, definitions and libraries */

#define NO_FLAGS 0  /* used for functions where no flag argument is used. */
#define IO_BUFF 512 /* max bytes that can be sent/recieved */
                           /* TODO: This is not max characters that user can
                            * input, if there is header information this
                            * includes that. Depends on how i implement other
                            * things.
                            */

typedef struct server_info{
    in_addr_t addr;      /* network binary of server address */
    char *dot_addr;      /* dotted representation of IP address */
    in_port_t port;      /* port used at IP address */
    int domain;          /* AF_INET or AF_INET6 */
    int sock_type;       /* IP protocol family, socket() definitions. */
    int sockfd;          /* socket file descriptior */
    struct sockaddr_in socket_info; /* socket API struct, IPV4 */
} struct_serv_info;

/*
typedef struct workspace_buffer{
    char buff[WORK_BUFF];
    size_t len;
} struct_work_buff;
*/

typedef struct stream_io{
    char   tx[IO_BUFF];
    size_t tx_len;

    char   rx[IO_BUFF];
    size_t rx_len;
} struct_io_buff;
    
/* TODO NOTE: basic layout. Nothing in stone, some of these may come from exceptions,
 * signals from other threads, etc. etc. */
int connect_to_server(struct_serv_info *serv_addr);
ssize_t send_to_server(int sockfd, char *tx, size_t len, int flags);
ssize_t recieve_from_server(int sockfd, char *rx, size_t len, int flags);
#endif
