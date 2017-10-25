/*******************************************************************************
 * Filename: client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

int connect_to_server(struct_serv_info *serv_info)
{
    int sockfd;
    void *sock_info = (void*) &serv_info->socket_info;
    socklen_t sock_info_size = sizeof(serv_info->socket_info);

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->sock_type, serv_info->pcol);
    if (_usrUnlikely(sockfd == FAILURE)){
        /* TODO: Retry to open socket until timeout */
        errMsg("connect_to_server: Failed to open the socket()");
        return FAILURE;
    }

    /* make connection */
    if (_usrUnlikely(connect(sockfd, sock_info, sock_info_size) == FAILURE)){
        /* TODO: error handle, retry connection till timeout. */
        errMsg("connect_to_server: Failed on connect() to server");
        return FAILURE;
    }

    serv_info->sockfd = sockfd;
    return SUCCESS;
} /* end connect to server */

/* returns number of bytes written to socket buffer, if error occures,
 * errno will be set to the appropriate error value from send(). */
ssize_t send_to_server(int sockfd, char *tx, size_t len, int flags)
{
    return socket_transmit(sockfd, tx, len, flags);
} /* end send_to_server */

ssize_t receive_from_server(int sockfd, char *rx, size_t len, int flags)
{
    return socket_receive(sockfd, rx, len, flags);
} /* end recieve_from_server */
