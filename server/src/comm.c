/*******************************************************************************
 * Filename: comm.c
 *
 * Written by: James Ross
 *
 * Handles all server connections and communication functions found in comm.h
 ******************************************************************************/

#include "comm.h"

int open_connection(struct_serv_info *serv_info)
{
    int sockfd;
    int ret;
    socklen_t sock_info_size = sizeof(struct sockaddr_in);

    /* open socket */
    sockfd = socket(serv_info->domain, serv_info->sock_type, serv_info->pcol);
    if (_usrUnlikely(sockfd == FAILURE)){
        /* TODO: Retry to open socket until timeout */
        err_msg("open_connection: Failed to open the socket()");
        return FAILURE;
    }

    /* bind socket */
    ret = bind(sockfd,(struct sockaddr*)serv_info->socket_info, sock_info_size);
    if (_usrUnlikely(ret == FAILURE)) {
        /* TODO: error handle, retry connection till timeout. */
        err_msg("open_connection: Failed on bind() socket");
        return FAILURE;
    }
    
    /* Go into connection-mode and accept incoming data */
    listen(sockfd, LQ_SIZE);

    serv_info->sockfd = sockfd;
    return SUCCESS;
} /* end open_connection() */

int init_server_comm(struct_serv_info *serv_info)
{
    struct sockaddr_in *addr_tmp = serv_info->socket_info;

    /* dot to binary representation */
    if(inet_pton(NET_DOMAIN, SERV_ADDR, &serv_info->addr) != 1){
        if(errno){
            errExit("irc_server: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_server: Invalid network address string");
    }
    
    /* init values for serv_info */
    serv_info->port      = htons(SERV_PORT);
    serv_info->domain    = NET_DOMAIN;
    serv_info->sock_type = SOCK_TYPE;
    serv_info->pcol      = IP_PROTOCOL;

    strncpy(serv_info->dot_addr, SERV_ADDR, SERV_LEN);

    /* init values for socket_info */
    addr_tmp->sin_port        = serv_info->port;
    addr_tmp->sin_family      = serv_info->domain; 
    addr_tmp->sin_addr.s_addr = serv_info->addr;

    /* open_connection sets serv_info->sockfd and initiates listen */
    if(_usrUnlikely(open_connection(serv_info) == FAILURE)) {
        errExit("irc_server: Initial connection to network failed");
    }

    return SUCCESS;
} /* end init_server_com() */

ssize_t send_to_client(int sockfd, uint8_t *tx, size_t len, int flags)
{
    if (!tx)
        return FAILURE;

    return socket_transmit(sockfd, tx, len, flags);
} /* end send_to_client */

ssize_t receive_from_client(int sockfd, uint8_t *rx, size_t len, int flags)
{
    if (!rx)
        return FAILURE;

    return socket_receive(sockfd, rx, len, flags);
} /* end recieve_from_client */


/*******************************************************************************
 * com_parse_cli_message
 *
 * TODO: Memory is not cleaned up on error yet.
 *       Also untested.
 ******************************************************************************/
struct_cli_message* com_parse_cli_message(uint8_t *rx)
{
    struct_cli_message *new_msg;
    int i, j;
    size_t len; 
    char tmp[_COM_IO_BUFF] = {'\0'};

    if (rx[0] == '\0') {
        noerr_msg("com_parse_cli_message: rx was empty. nothing to pasrse");
        return NULL;
    }

    new_msg = CALLOC(struct_cli_message);
    if (!new_msg) {
        err_msg("com_parse_cli_message: new_msg calloc failure.");
        return NULL;
    }
   
    /* get name */ /* TODO: check name length in case client didnt. */
    for (i=0; rx[i] != '\0'; ++i)
        tmp[i] = rx[i];
    ++i; /* increment passed the '\0', handled in CALLOC below */

    /* store name */
    len = strlen(tmp)+1;
    new_msg->cli_name = CALLOC_ARRAY(char, len);
    if (!new_msg->cli_name) {
        err_msg("com_parse_cli_message: cli_name calloc fail.");
        return NULL;
    }

    strncpy(new_msg->cli_name, tmp, len);

    /* parse type, i is currently at next index of rx, type is 1 bytes */
    new_msg->type = rx[i];
    ++i; /* move past type index */

    /* parse msg if applicable, copies '\0' */
    memset(tmp, '\0', MSG_TYPE_SIZE); 
    for (j=0; rx[i] != '\r'; ++j, ++i)
        tmp[j] = rx[i];
    
    /* store message if aplicable */
    len = strlen(tmp) + 1;
    new_msg->msg = CALLOC_ARRAY(char, len);
    strncpy(new_msg->msg, tmp, len); 
   
    return new_msg;
} /* end parse_cli_message() */


int com_send_logon_result(int fd, int payload)
{
    int ret;
    uint8_t tx[_LOGON_REPLY_SIZE] = {RC_LOGON, payload, '\r'};
    printf("sending logon result.\n");
    ret = send_to_client(fd, tx, sizeof(tx), NO_FLAGS);
    if (ret == FAILURE) {
        printf("sending logon result FAILED.\n");
        return FAILURE;
    }
    printf("sending logon result SUCCESS.\n");
    return SUCCESS;
} /* end com_send_logon_result */
/******* EOF *******/
