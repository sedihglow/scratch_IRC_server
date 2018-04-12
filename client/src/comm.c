/*******************************************************************************
 * Filename: comm.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../../shared/include/irc_comm.h"
#include "../include/comm.h"

struct_serv_info* com_init_serv_info(void)
{
    struct_serv_info *init;

    init = CALLOC(struct_serv_info);
    if (!init) {
        err_msg("com_init_serv_info: calloc failure - struct_serv_info");
        return NULL;
    }
    
    /* init dot representation address */
    init->dot_addr = CALLOC_ARRAY(char, _COM_SERV_LEN);
    if (!init->dot_addr) {
        err_msg("com_init_serv_info: calloc failure - dot_addr");
        return NULL;
    }

    init->socket_info = CALLOC(struct sockaddr_in);
    if (!init->socket_info) {
        err_msg("com_init_serv_info: calloc failure - socket_info");
        return NULL;
    }
    
    /* non allocated initializations */
    init->addr      = 0;
    init->port      = 0;
    init->domain    = 0;
    init->sock_type = 0;
    init->pcol      = 0;
    init->sockfd    = 0;

    return init;
}/* end com_init_serv_info */

struct_io_ring* com_init_io_buff() 
{
    struct_io_ring *init;

    init = CALLOC(struct_io_ring);
    if (!init) {
        err_msg("com_init_io_buff: calloc failure - struct_io_ring");
        return NULL;
    }

    init->buff = CALLOC_ARRAY(char, IO_RING_SIZE);
    if (!init->buff) {
        err_msg("com_init_io_buff: calloc failure - buff");
        return NULL;
    }
    
    /* non allocated initializations */
    init->start = 0;
    init->end   = 0;
    
    return init;
} /* end com_init_io_buff */

// TODO: Might just make this a display, only local to client, not to history
int request_who(char *name, bool *online)
{
    return false;
} /* end request_who */

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
    socket_info = serv_info->socket_info;

    /* init serv_info  */
    serv_info->domain    = _COM_NET_DOMAIN;
    serv_info->pcol      = _COM_IP_PROTOCOL;
    serv_info->sock_type = _COM_SOCK_TYPE;

    /* dot to binary representation */
    if (inet_pton(_COM_NET_DOMAIN, _COM_SERV_ADDR, &serv_info->addr) != 1) {
        if (errno) {
            errExit("irc_client: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_client: Invalid network address string");
    }


    strncpy(serv_info->dot_addr, _COM_SERV_ADDR, strlen(_COM_SERV_ADDR)+1);

    /* set socket address information for struct sockAddr_in */
    socket_info->sin_family      = _COM_NET_DOMAIN;
    socket_info->sin_addr.s_addr = serv_info->addr;

    /* convert port to network order */
    socket_info->sin_port = serv_info->port = htons(_COM_SERV_PORT);

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
