/*******************************************************************************
 * Filename: irc_comm.h
 *
 * These are functions and definitions that can be used for both the client
 * and the server programs. 
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _IRC_COMM_H_
#define _IRC_COMM_H_

#include "utility_sys.h" /* useful macros, definitions and libraries */

#define NO_FLAGS 0  /* used for functions where no flag argument is used. */

/* Server connectivity information */
#define _COM_SERV_ADDR   "10.0.0.169"
#define _COM_SERV_LEN    sizeof(_COM_SERV_ADDR)
#define _COM_SERV_PORT   50059 /* port listening on server */
#define _COM_NET_DOMAIN  AF_INET        /* network domain we are using. IPV4 */
#define _COM_SOCK_TYPE   SOCK_STREAM    /* tcp socket */
#define _COM_IP_PROTOCOL 0              /* Default for type in socket() */

#define _COM_IO_BUFF 512 /* max bytes that can be sent/recieved */
                           /* TODO: This is not max characters that user can
                            * input, if there is header information this
                            * includes that. Depends on how i implement other
                            * things.
                            */

#define IO_RING_SIZE 500 // size of ring buffer

#define _NAME_SIZE_MAX  11    // includes '\0'

#define MSG_TYPE_SIZE 1

/******************************************************************************
 *                      Command Code Definition
 *                  TODO: Not all implemented, was idealism.
 ******************************************************************************/
#define RC_FA     0x1
#define RC_FL     0x2
#define RC_BL     0x3
#define RC_BA     0x4
#define RC_BR     0x5
#define RC_WHO    0x6
#define RC_JOIN   0x7
#define RC_EXIT   0x8
#define RC_VOID   0x9
#define RC_LOGOUT 0xA
#define RC_INV    0xB
#define RC_RL     0xC
#define RC_PM     0xD
#define RC_PR     0xE
#define RC_ERR    0xF
#define RC_FR     0x10
#define RC_LOGON  0x11
#define RC_R_MSG  0x12
#define RC_HB     0x14
#define RC_LEAVE  0x15
#define RC_MSG    0x16
#define RC_RUL    0x17 /* list users in a room */

#define RESERVED_Z 0x0  /* was useful to reserve for error checking */
#define RESERVE_CR 0x13 /* reserved since we use \r in messages */

/* server to client reply success/failure */
#define _REPLY_SUCCESS 1
#define _REPLY_FAILURE 0

/* length of the reply to logon. */
#define _LOGON_REPLY_SIZE 3

typedef struct server_info {
    in_addr_t addr;      /* network binary of server address */
    char *dot_addr;      /* dotted representation of IP address */
    in_port_t port;      /* port used at IP address, network ordered */
    int domain;          /* AF_INET or AF_INET6 */
    int sock_type;       /* type of socket, socket() definition. */
    int pcol;            /* Protocol argument used in socket() */
    int sockfd;          /* socket file descriptior */
    struct sockaddr_in *socket_info; /* socket API struct, IPV4 */
} struct_serv_info;

typedef struct parsed_cli_message {
    char *cli_name;
    int  type;
    char *msg;
} struct_cli_message;

typedef struct parsed_serv_message {
    uint8_t type;
    char *msg;
} struct_serv_message;

/****************************************************************************** 
 * DEVELOPMENT NOTE: This is a generic ring buffer but it is IO since COM
 *                   should be the only thing messing with struct_io_ring
 *
 *                   If not, update this comment and the name to generic ring
 *                   buffer?
 *
 * make use of tx and rx as a ring buffer 
 ******************************************************************************/
typedef struct stream_io{
    char   *buff;
    size_t start;
    size_t end;
} struct_io_ring;

struct_serv_info* _com_init_serv_info(void);
struct_io_ring* _com_init_io_ring(void);
void _com_free_serv_info(struct_serv_info *dest);
void _com_free_io_ring(struct_io_ring *dest);
void _com_free_cli_message(struct_cli_message *rem);
void _com_free_serv_message(struct_serv_message *rem);
/******************************************************************************* 
 * TODO: Maybe make these functions since they are long, though my current
 * protocol with them just calls a seperate stack frame and just calls the
 * inline, which the compiler should notice and just make that 1 stack frame
 * this code...
 *
 * An analyze binary size, which i think is okay since it is only called in a
 * single line function that returns this. Other issues though? Memory
 * imprint in a different way?
 ******************************************************************************/
static inline ssize_t socket_transmit(int sockfd, uint8_t *tx, 
                                      size_t len, int flags)
{
    ssize_t sent;            /* number of bytes written to socket */
    size_t  remaining = len; /* number of bytes left to write */

 //   while (remaining > 0) {
        sent = send(sockfd, tx, remaining, flags);
        if (_usrUnlikely(sent == FAILURE)) {
            err_msg("socket_transmit: send() failed");
            return FAILURE;
        }

        /* in case there was something not written, try again */
        remaining -= sent;
        tx        += sent; 
//    }

   return (len - remaining);
} /* end socket_transmit */

static inline ssize_t socket_receive(int sockfd, uint8_t *rx, 
                                     size_t len, int flags)
{
    ssize_t received = 1;    /* bytes read from a socket, non-EOF init */
    size_t  remaining = len; /* bytes still in buffer */

    //while (remaining > 0) { Would be okay if all static length messages
/* TODO: com_get_logon_result blocking will test if we need that while loop */
        received = recv(sockfd, rx, remaining, flags);
        if (received == FAILURE) {
            err_msg("socket_recieve: recv() failed");
            return FAILURE;
        } 

/*
        if (received == 0)
            break;
*/
        remaining -= received;
        rx        += received;
    //}
    
    return (len - remaining);
} /* end socket_recieve */
#endif
