/*******************************************************************************
 * Filename: server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../include/server.h"

int open_connection(struct_serv_info *serv_info)
{
    int sockfd;
    void *sock_info = (void *) &serv_info->socket_info;
    socklen_t sock_info_size = sizeof(serv_info->socket_info);

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->sock_type, serv_info->pcol);
    if (_usrUnlikely(sockfd == FAILURE)){
        /* TODO: Retry to open socket until timeout */
        err_msg("open_connection: Failed to open the socket()");
        return FAILURE;
    }

    /* bind socket */
    if (_usrUnlikely(bind(sockfd, sock_info, sock_info_size) == FAILURE)){
        /* TODO: error handle, retry connection till timeout. */
        err_msg("open_connection: Failed on bind() socket");
        return FAILURE;
    }
    
    /* go into connection-mode and accept incoming data */
    listen(sockfd, LQ_SIZE);

    serv_info->sockfd = sockfd;
    return SUCCESS;
}

ssize_t send_to_client(int sockfd, char *tx, size_t len, int flags)
{
    return socket_transmit(sockfd, tx, len, flags);
} /* end send_to_client */

ssize_t receive_from_client(int sockfd, char *rx, size_t len, int flags)
{
    return socket_receive(sockfd, rx, len, flags);
} /* end recieve_from_client */
