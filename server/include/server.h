/*******************************************************************************
 * Filename: server.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_comm.h"


/* servers information on the client it is communicating with. */
typedef struct client_info{
    int sockfd;
    struct sockaddr_in socket_info;
}struct_cli_info;

