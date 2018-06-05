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
    if (rx[i] == '\r')
        return new_msg;

    /* parse msg if applicable, copies '\0' */
    len = strlen(tmp)+1;
    memset(tmp, '\0', len); 
    for (j=0; rx[i] != '\r'; ++j, ++i)
        tmp[j] = rx[i];
    
    /* store message if aplicable */
    len = j+1; /* length of final payload */
    new_msg->msg = CALLOC_ARRAY(char, len);
    for (j=0; j < len; ++j)
        new_msg->msg[j] = tmp[j];

    return new_msg;
} /* end parse_cli_message() */

/* server to client format: type | 1/0 | '\r' */
int com_send_logon_result(int fd, uint8_t payload)
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


static int com_send_join_leave_result(int fd, char *room_name, uint8_t num_users,
                                      uint8_t res, uint8_t type);

static int com_send_join_leave_result(int fd, char *room_name, uint8_t num_users, 
                                      uint8_t res, uint8_t type)
{
    int len, i, j;
    uint8_t tx[IO_BUFF] = {'\0'};
   
    if (!room_name) {
        errnum_msg(EINVAL, "com_send_join_result: room name was NULL.");
        return FAILURE;
    }

    i = 0;
    if (type == RC_JOIN) {
        tx[i] = RC_JOIN;
        ++i;
        tx[i] = num_users;
    }
    else if (type == RC_LEAVE) {
        tx[i] = RC_LEAVE;
    }
    else {
        return FAILURE; /* just catches programming errors. */
    }
    ++i;
    
    len = strlen(room_name) + 1;
    for (j=0; j < len; ++i, ++j)
        tx[i] = room_name[j];

    tx[i] = res;
    ++i;
    tx[i] = '\r';

    return send_to_client(fd, tx, i+1, NO_FLAGS);
} /* end com_send_join_leave_result */


/*
 * TODO: if more time would send all the users so it could populate and
 *       be persistantly displayed w/ updates
 *
 * server to client format: RC_JOIN | num_users | room_name  | 1/0 | '\r'
 */
int com_send_join_result(int fd, char *room_name, uint8_t num_users, uint8_t res)
{
    return com_send_join_leave_result(fd, room_name, num_users, res, RC_JOIN);
} /* end com_send_join_result */

/*
 * server to cli format: RC_LEAVE | room_name | res | \r
 */
int com_send_leave_result(int fd, char *room_name, uint8_t res)
{
    return com_send_join_leave_result(fd, room_name, 0, res, RC_LEAVE);
} /* end com_send_leave_result */

/*
 * server to cli format: RC_MSG | room_name | client name | msg | \r
 */
int com_send_room_message(int fd, char *room_name, char *cli_name, char *msg)
{
    int len, i;
    uint8_t tx[IO_BUFF] = {'\0'};

    assert(cli_name && room_name && msg);

    i = 0;
    tx[i] = RC_MSG;
    ++i;

    /* copy room name */
    len = strlen(room_name) + 1;
    strncpy((char*)(tx+i), room_name, len);
    i += len;

    /* copy client name */
    len = strlen(cli_name) + 1;
    strncpy((char*)(tx+i), cli_name, len);
    i += len; 

    /* copy message */
    len = strlen(msg) + 1;
    strncpy((char*)(tx+i), msg, len);
    i += len;

    tx[i] = '\r';

    return send_to_client(fd, tx, i+1, NO_FLAGS);
} /* end com_send_room_message */

void com_send_exit_message(int fd) 
{
    uint8_t tx[] = {RC_EXIT, '\r'};
    send_to_client(fd, tx, sizeof(tx), NO_FLAGS);
} /* end com_send_exit_message */

/* 
 * If user_name is null, format: RC_RUL | \r
 * else format : RC_RUL | user name | \r
 */
int com_send_room_user_messsage(int fd, char *user_name)
{
    uint8_t tx[IO_BUFF] = {'\0'};
    int len, i;
    
    i = 0;
    tx[i] = RC_RUL;
    ++i;

    if (!user_name) {
        tx[i] = '\r';
        return send_to_client(fd, tx, i+1, NO_FLAGS); 
    }
    
    len = strlen(user_name) + 1;
    strncpy((char*)(tx+i), user_name, len);
    i += len;

    tx[i] = '\r';

    return send_to_client(fd, tx, i+1, NO_FLAGS); 
} /* end com_send_room_user_message */


/******* EOF *******/
