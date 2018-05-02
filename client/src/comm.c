/*******************************************************************************
 * Filename: comm.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../../shared/include/irc_comm.h"
#include "../include/comm.h"

struct_serv_info* com_init_serv_info(void)
{
    return _com_init_serv_info();
}/* end com_init_serv_info */

struct_io_ring* com_init_io_ring(void) 
{
    return _com_init_io_ring();
} /* end com_init_io_buff */


void com_free_serv_info(struct_serv_info *dest)
{
    _com_free_serv_info(dest);
}

void com_free_io_ring(struct_io_ring *dest)
{
    _com_free_io_ring(dest);
}

// TODO: Might just make this a display, only local to client, not to history
int request_who(char *name, bool *online)
{
    return false;
} /* end request_who */

static int connect_to_server(struct_serv_info *serv_info)
{
    int ret;
    int sockfd;
    socklen_t socklen = sizeof(struct sockaddr_in);

    if (serv_info == NULL) {
        errExit("connect_to_server: server info was NULL");
        return EINVAL;
    }

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->sock_type, serv_info->pcol);
    if (_usrUnlikely(sockfd == FAILURE)) {
        /* TODO: Retry to open socket until timeout */
        err_msg("connect_to_server: Failed to open the socket()");
        return FAILURE;
    }

    /* make connection */
    ret = connect(sockfd, (struct sockaddr*)serv_info->socket_info, socklen);
    if (_usrUnlikely(ret == FAILURE)) {
        /* TODO: error handle, retry connection till timeout. */
        err_msg("connect_to_server: Failed on connect() to server");
        return FAILURE;
    }

    serv_info->sockfd = sockfd;
    return SUCCESS;
} /* end connect to server */

int init_client_comm(struct_serv_info *serv_info)
{
    struct sockaddr_in *addr_tmp = serv_info->socket_info;

    /* dot to binary representation */
    if (inet_pton(_COM_NET_DOMAIN, _COM_SERV_ADDR, &serv_info->addr) != 1) {
        if (errno) {
            errExit("irc_client: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_client: Invalid network address string");
    }

    /* init serv_info  */
    serv_info->domain    = _COM_NET_DOMAIN;
    serv_info->pcol      = _COM_IP_PROTOCOL;
    serv_info->sock_type = _COM_SOCK_TYPE;
    /* convert port to network order */
    serv_info->port = htons(_COM_SERV_PORT);

    strncpy(serv_info->dot_addr, _COM_SERV_ADDR, strlen(_COM_SERV_ADDR));

    /* init socket_info values */
    addr_tmp->sin_family = serv_info->domain;
    addr_tmp->sin_addr.s_addr = serv_info->addr;
    addr_tmp->sin_port = serv_info->port;


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
