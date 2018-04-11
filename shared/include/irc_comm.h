/*******************************************************************************
 * Filename: irc_utility.h
 *
 * These are functions and definitions that can be used for both the client
 * and the server programs. These functions should not require specific
 * knowlege of client vs server utilization.
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _IRC_UTILITY_H_
#define _IRC_UTILITY_H_

#include "utility_sys.h" /* useful macros, definitions and libraries */

#define NO_FLAGS 0  /* used for functions where no flag argument is used. */

/* Server connectivity information */
#define SERV_ADDR   "10.0.0.171"
#define SERV_LEN    11//sizeof(SERV_ADDR)
#define SERV_PORT   60000             /* port listening on server */
#define NET_DOMAIN  AF_INET           /* network domain we are using. IPV4 */
#define SOCK_TYPE   SOCK_STREAM       /* tcp socket */
#define IP_PROTOCOL 0                 /* Default for type in socket() */

#define IO_BUFF 512 /* max bytes that can be sent/recieved */
                           /* TODO: This is not max characters that user can
                            * input, if there is header information this
                            * includes that. Depends on how i implement other
                            * things.
                            */

typedef struct server_info {
    in_addr_t addr;      /* network binary of server address */
    char *dot_addr;      /* dotted representation of IP address */
    in_port_t port;      /* port used at IP address, network ordered */
    int domain;          /* AF_INET or AF_INET6 */
    int sock_type;       /* type of socket, socket() definition. */
    int pcol;            /* Protocol argument used in socket() */
    int sockfd;          /* socket file descriptior */
    struct sockaddr_in socket_info; /* socket API struct, IPV4 */
} struct_serv_info;


/*
typedef struct workspace_buffer{
    char buff[WORK_BUFF];
    size_t len;
} struct_work_buff;
*/

/* make use of tx and rx as a ring buffer */
typedef struct stream_io{
    char   buff[IO_BUFF];
    size_t start;
    size_t end;
} struct_io_ring;

static inline ssize_t socket_transmit(int sockfd, char *tx, 
                                      size_t len, int flags)
{
    ssize_t sent;            /* number of bytes written to socket */
    size_t  remaining = len; /* number of bytes left to write */

    while (remaining > 0) {
        sent = send(sockfd, tx, remaining, flags);
        if (_usrUnlikely(sent == FAILURE)) {
            err_msg("socket_transmit: send() failed");
            return FAILURE;
        }

        /* in case there was something not written, try again */
        remaining -= sent;
        tx        += sent; 
    }

   return (len - remaining);
} /* end socket_transmit */

static inline ssize_t socket_receive(int sockfd, char *rx, 
                                     size_t len, int flags)
{
    ssize_t received = 1;    /* bytes read from a socket, non-EOF init */
    size_t  remaining = len; /* bytes still in buffer */

    while (remaining > 0) {
        received = recv(sockfd, rx, remaining, flags);
        if (_usrUnlikely(received == FAILURE)) {
            err_msg("socket_recieve: recv() failed");
            return FAILURE;
        } else if (received == 0) {
            break;
        }

        remaining -= received;
        rx        += received;
    }
    
    return (len - remaining);
} /* end socket_recieve */
#endif
