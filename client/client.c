/*******************************************************************************
 * Client side operations of a basic, from scratch, irc server.
 *
 * Filename: client.c
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

int connect_to_server(struct_serv_info *serv_info)
{
    int sockfd;
    struct sockaddr *sock_info = (struct sockaddr*) &serv_info->socket_info;
    socklen_t sock_info_size = sizeof(sock_info);

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->domain, serv_info->sock_type);
    if (_usrUnlikely(sockfd == FAILURE)){
        /* TODO: Retry to open socket until timeout */
        errMsg("connect_to_server: Failed to open the socket()\n");
        return FAILURE;
    }

    /* make connection */
    if (_usrUnlikely(connect(sockfd, sock_info, sock_info_size) == FAILURE)){
        /* TODO: error handle, retry connection till timeout. */
        errMsg("connect_to_server: Failed on connect() to server\n");
        return FAILURE;
    }

    serv_info->sockfd = sockfd;
    return SUCCESS;
} /* end connect to server */

/* returns number of bytes written to socket buffer, if error occures,
 * errno will be set to the appropriate error value from send(). */
ssize_t send_to_server(int sockfd, char *tx, size_t len, int flags)
{
    ssize_t sent;           /* number of bytes written to socket */
    size_t remaining = len; /* number of bytes left to write */

    while(remaining > 0){
        sent = send(sockfd, tx, remaining, flags);
        if(_usrUnlikely(sent == FAILURE)){
            return FAILURE;
        }

        /* in case there was something not written, try again */
        remaining -= sent;
        tx        += sent; 
    }

   return (len - remaining);
} /* end send_to_server */

ssize_t recieve_from_server(int sockfd, char *rx, size_t len, int flags)
{
    ssize_t received;       /* number of bytes read from a socket */
    size_t remaining = len; /* number of bytes still in buffer */

    while(remaining > 0 && received != EOF){
        received = recv(sockfd, rx, remaining, flags);
        if(_usrUnlikely(received == FAILURE)){
            return FAILURE;
        }

        remaining -= received;
        rx        += received;
    }
    
    return (len - remaining);
} /* end recieve_from_server */
