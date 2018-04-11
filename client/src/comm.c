/*******************************************************************************
 * Filename: comm.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../../shared/include/irc_comm.h"
#include "../include/comm.h"

static int connect_to_server(struct_serv_info *serv_info)
{
    int sockfd;
    void *sock_info = (void*) &serv_info->socket_info;
    socklen_t sock_info_size = sizeof(serv_info->socket_info);

    if (serv_info == NULL) {
        errExit("connect_to_server: server info was NULL");
        return EINVAL;
    }

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->sock_type, serv_info->pcol);
    if (_usrUnlikely(sockfd == FAILURE)){
        /* TODO: Retry to open socket until timeout */
        err_msg("connect_to_server: Failed to open the socket()");
        return FAILURE;
    }

    /* make connection */
    if (_usrUnlikely(connect(sockfd, sock_info, sock_info_size) == FAILURE)){
        /* TODO: error handle, retry connection till timeout. */
        err_msg("connect_to_server: Failed on connect() to server");
        return FAILURE;
    }

    serv_info->sockfd = sockfd;
    return SUCCESS;
} /* end connect to server */

int init_client_comm(struct_serv_info *serv_info)
{
    struct sockaddr_in *socket_info;

    /* set a pointer for easier access to socket_info */
    socket_info = &(serv_info->socket_info);

    /* init serv_info  */
    serv_info->domain    = NET_DOMAIN;
    serv_info->pcol      = IP_PROTOCOL;
    serv_info->sock_type = SOCK_TYPE;

    /* dot to binary representation */
    if (inet_pton(NET_DOMAIN, SERV_ADDR, &serv_info->addr) != 1) {
        if (errno) {
            errExit("irc_client: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_client: Invalid network address string");
    }

    /* init dot representation address */
    serv_info->dot_addr = CALLOC_ARRAY(char, SERV_LEN);
    if(!serv_info->dot_addr)
        errExit("irc_client: malloc failure dot_addr");

    strncpy(serv_info->dot_addr, SERV_ADDR, strlen(SERV_ADDR)+1);

    /* set socket address information for struct sockAddr_in */
    socket_info->sin_family      = NET_DOMAIN;
    socket_info->sin_addr.s_addr = serv_info->addr;

    /* convert port to network order */
    socket_info->sin_port = serv_info->port = htons(SERV_PORT);

    /* connect_to_server() sets serv_info->sockfd */
    if(connect_to_server(serv_info) == FAILURE)
        errExit("irc_client: Initial connection to server failed");

    return SUCCESS;
} /* end init_client_comm */

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

/******* EOF ******/
