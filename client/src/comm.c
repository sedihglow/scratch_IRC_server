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
} /* end com_free_serv_info */

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

/******************************************************************************
 * TODO:
 *
 * Memory not cleaned up on exit. 
 192.168.3.133*
 * Replace errExit functions and clean up memory in irc_server before killing
 * program
 ******************************************************************************/
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
ssize_t send_to_server(int sockfd, uint8_t *tx, size_t len, int flags)
{
    return socket_transmit(sockfd, tx, len, flags);
} /* end send_to_server */

ssize_t receive_from_server(int sockfd, uint8_t *rx, size_t len, int flags)
{
    return socket_receive(sockfd, rx, len, flags);
} /* end recieve_from_server */

int com_send_logon_message(char *name, struct_serv_info *serv_info)
{
    uint8_t *msg;
    size_t name_len;
    size_t msg_len;
     
    name_len = strlen(name) + 1;
    msg_len = name_len + MSG_TYPE_SIZE + 1; 

    msg = CALLOC_ARRAY(uint8_t, msg_len);
    if (!msg)
        errExit("send_logon_message: download more ram.");

    strncpy((char*)msg, name, name_len);  /* copy name w/ '\0' */
    
    msg[name_len] = RC_LOGON; /* add type */
    msg[name_len+1] = '\r';   /* add terminator */

    if (send_to_server(serv_info->sockfd, msg, msg_len, NO_FLAGS) == FAILURE) {
        free(msg);
        return FAILURE;
    }

    free(msg);
    return SUCCESS;
} /* end com_send_logon_message */

/* server reply format : type | 1/0 | '\r' */
int com_get_logon_result(int fd)
{
    uint8_t rx[IO_BUFF] = {'\0'};

    usleep(1);

    /* should... block until data is recieved. */
    receive_from_server(fd, rx, IO_BUFF, NO_FLAGS);

    if (rx[0] != RC_LOGON || rx[1] == 0x0)
        return FAILURE;

    return SUCCESS;
} /* end com_get_logon_message */

int com_send_chat_message(char *cli_name, char *room_name, char *msg, 
                          struct_serv_info *serv_info)
{
    uint8_t *tx;
    int i;
    size_t name_len = strlen(cli_name) + 1;
    size_t msg_len  = strlen(msg)  + 1;
    size_t room_len = strlen(room_name) + 1;
    size_t tx_len = MSG_TYPE_SIZE + name_len + room_len + msg_len + 1;

    tx = CALLOC_ARRAY(uint8_t, tx_len);
    if (!tx)
        errExit("Failed to allocate TX, download more ram.");
   
    i = 0;
    strncpy((char*)(tx+i), cli_name, name_len);
    i += name_len;
    tx[i] = RC_MSG;
    ++i;
    
    strncpy((char*)(tx+i), room_name, room_len);
    i += room_len;

    strncpy((char*)(tx+i), msg, msg_len);
    i += msg_len;
    tx[i] = '\r';
    
    send_to_server(serv_info->sockfd, tx, tx_len, NO_FLAGS);
    
    free(tx);
    return SUCCESS;
} /* end com_send_chat_message */


static int com_join_leave_send(char *cli_name, char *room_name,
                               struct_serv_info *serv_info, int type);


static int com_join_leave_send(char *cli_name, char *room_name,
                               struct_serv_info *serv_info, int type)
{
    uint8_t *tx;
    int i;
    size_t name_len  = strlen(cli_name)  + 1;
    size_t room_len  = strlen(room_name) + 1;
    size_t tx_len = name_len + MSG_TYPE_SIZE + room_len + 1;

    if (room_name[0] == '\0') {
        noerr_msg("com_join_leave_send: room name was '\0'. Invalid.");
        return FAILURE;
    }

    tx = CALLOC_ARRAY(uint8_t, tx_len);
    if (!tx)
        errExit("com_send_join_request: tx could not calloc");
   
    /* set client name field */
    i = 0;
    strncpy((char*)(tx+i), cli_name, name_len);
    i += name_len;

    /* set type byte */
    if (type == RC_JOIN)
        tx[i] = RC_JOIN;
    else if (type == RC_LEAVE)
        tx[i] = RC_LEAVE;
    else
        return FAILURE;
    ++i;
    
    /* set room name payload */
    strncpy((char*)(tx+i), room_name,  room_len);
    i += room_len;
    tx[i] = '\r';

    send_to_server(serv_info->sockfd, tx, tx_len, NO_FLAGS);

    free(tx);
    return SUCCESS;
} /* end com_join_leave_send */

int com_send_join_message(char *cli_name, char *room_name,  
                          struct_serv_info *serv_info)
{
    return com_join_leave_send(cli_name, room_name, serv_info, RC_JOIN);
} /* end com_send_join_request */

int com_send_leave_message(char *cli_name, char *room_name, 
                           struct_serv_info *serv_info)
{
    return com_join_leave_send(cli_name, room_name, serv_info, RC_LEAVE);
} /* end com_send_leave_message */


static int com_logout_exit_send(char *cli_name, struct_serv_info *serv_info,
                                int type);

static int com_logout_exit_send(char *cli_name, struct_serv_info *serv_info,
                                int type) 
{
    uint8_t *tx;
    int i;
    size_t name_len = strlen(cli_name) + 1;
    size_t tx_len  = name_len + MSG_TYPE_SIZE + 1;

    tx = CALLOC_ARRAY(uint8_t, tx_len);
    if (!tx)
        errExit("com_send_join_request: tx could not calloc");
  
    /* set client name field */
    i = 0;
    strncpy((char*)(tx+i), cli_name, name_len);
    i += name_len;

    /* set type byte */
    if (type == RC_EXIT)
        tx[i] = RC_EXIT;
    else if (type == RC_LOGOUT)
        tx[i] = RC_LOGOUT;
    else
        return FAILURE;
    ++i;
    tx[i] = '\r';

    send_to_server(serv_info->sockfd, tx, tx_len, NO_FLAGS);

    free(tx);
    return SUCCESS;
} /* end com_logout_exit_send */

int com_send_exit_message(char *cli_name, struct_serv_info *serv_info)
{
    return com_logout_exit_send(cli_name, serv_info, RC_EXIT);
} /* end com_send_exit_message */

/*
 * Client to server format: cli_name | type | room name | \r
 */
int com_send_room_users_message(char *cli_name, char *room_name, 
                        struct_serv_info *serv_info)
{
    uint8_t *tx;
    int i;
    size_t name_len = strlen(cli_name) + 1;
    size_t room_len = strlen(room_name) + 1;
    size_t tx_len = MSG_TYPE_SIZE + name_len + room_len + 1;

    tx = CALLOC_ARRAY(uint8_t, tx_len);
    if (!tx)
        errExit("com_send_join_request: tx could not calloc");

    i = 0;
    strncpy((char*)(tx+i), cli_name, name_len);
    i += name_len;

    tx[i] = RC_RUL;
    ++i;
    strncpy((char*)(tx+i), room_name, room_len);
    i += room_len;

    tx[i] = '\r';

    send_to_server(serv_info->sockfd, tx, tx_len, NO_FLAGS);

    return SUCCESS;
} /* end com_send_exit_message */


struct_serv_message* com_parse_server_msg(uint8_t *input)
{
    struct_serv_message *serv_msg;
    char tmp[IO_BUFF] = {'\0'};
    int i, j, len;

    if (!input) {
        errnum_msg(EINVAL, "input was null or empty");
        return NULL;
    }

    if (input[0] == RESERVED_Z) {
        err_msg("com_parse_server_msg: type sent is reserved, ignoring.");
        return NULL;
    }

    serv_msg = CALLOC(struct_serv_message);
    if (!serv_msg)
        errExit("com_parse_server_msg: serv_msg failed to calloc");

    i = 0;
    serv_msg->type = input[i];
    ++i;
    if (input[i] == '\r')
        return serv_msg;
    
    /* copy payload into tmp */
    for(j=0; input[i] != '\r'; ++i, ++j)
        tmp[j] = input[i];
    
    len = j + 1;
    serv_msg->msg = CALLOC_ARRAY(char, len);
    if (!serv_msg->msg)
        errExit("com_parse_server_msg: serv payload failed calloc");
    
    for (i=0; i < len; ++i)
        serv_msg->msg[i] = tmp[i];

    return serv_msg;
} /* end com_parse_server_msg */





/* 
 *  com_send_logout_message
 *
 * TODO: This should not be used since not implemented on server side until
 *       persistent user credentials happen.
int com_send_logout_message(char *cli_name, struct_serv_info *serv_info)
{
    return com_logout_exit_send(cli_name, serv_info, RC_LOGOUT);
}
*/
/******* EOF ******/
