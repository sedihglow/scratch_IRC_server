/*******************************************************************************
 * Filename: irc_server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_server.h"


/*******************************************************************************
 *  irc_fill_fd_set_read
 *
 ******************************************************************************/
void irc_fill_fd_set_read(struct_irc_info *irc_info, fd_set *readfds)
{
    int i;
    int size;
    int *list;

    if (!irc_info || !readfds) {
        errnum_msg(EINVAL, "fill_fd_set_read: info or readfs was NULL.");
        return;
    } else if (!irc_info->full_fd_list) {
        return;
    }

    FD_ZERO(readfds);

    list = irc_info->full_fd_list;
    size = irc_info->num_fds;
    for (i=0; i < size; ++i)
        FD_SET(list[i], readfds);
} /* end fill_fd_set_read() */

/*******************************************************************************
 * irc_add_to_fd_list
 *
 * Same functionality as the server add client but with file descriptor lists.
 ******************************************************************************/
int* irc_add_fd_list(struct_irc_info *info, int newfd)
{
    unsigned int i;
    int *new_list;
    int *old_list;
    size_t old_size;

    if (!info) {
        errno = EINVAL;
        errnum_msg(EINVAL, "irc_add_fd_list: info == NULL");
        return info->full_fd_list;
    }

    if (!info->full_fd_list && info->num_fds == 0) {
        new_list = CALLOC(int);
        new_list[0] = newfd;
        ++(info->num_fds);
        return new_list;
    }
    
    old_list = info->full_fd_list;
    old_size = info->num_fds;
    new_list = CALLOC_ARRAY(int, info->num_fds + 1);

    for (i=0; i < old_size; ++i)
        new_list[i] = old_list[i];

    new_list[i] = newfd;

    ++(info->num_fds);

    free(info->full_fd_list);
    info->full_fd_list = NULL;

    return new_list;
} /* end add_to_fd_list() */

/*******************************************************************************
 *  sets full_fd_list to null if deallocation happens
 *
 *  on error, errno is set and it returns the previous list in info
 ******************************************************************************/
int* irc_remove_fd_list(struct_irc_info *info, int fd)
{
    unsigned int i, j;
    int *new_list;
    int *old_list;
    size_t old_size;

    if (!info) {
        errno = EINVAL;
        errnum_msg(EINVAL, "irc_remove_fd_list: info == NULL");
        return info->full_fd_list;
    }

    if (!info->full_fd_list && info->num_fds == 0) {
        noerr_msg("num_fd is 0 or full_fd_list was null and empty");
        return info->full_fd_list;
    }

    if (info->num_fds == 1) {
        free(info->full_fd_list);
        info->full_fd_list = NULL;
        return info->full_fd_list;
    }

    old_list = info->full_fd_list;
    old_size = info->num_fds;
    new_list = CALLOC_ARRAY(int, info->num_fds - 1);

    j=0;
    for (i=0; i < old_size; ++i) {
        if(old_list[i] != fd) {
            new_list[j] = old_list[i];
            ++j;
        }
    }

    free(info->full_fd_list);
    info->full_fd_list = NULL;

    --(info->num_fds);

    return new_list;
} /* irc_remove_fd_list() */  

/*******************************************************************************
 *  irc_init_info
 *
 *  initializes the struct irc_info.
 *
 *  full_fd_list and cli_list remains NULL since it is handled in the add/remove
 *  functions.
 ******************************************************************************/
struct_irc_info* irc_init_info(void)
{
    /* 
     * full_fd_list allocation is handled in adding/removing from list. 
     * cli_list is allocated the same way.
     */
    struct_irc_info *info;

    info = CALLOC(struct_irc_info);
    if (!info) {
        err_msg("irc_init_info: info failed to allocate.");
        return NULL;
    }
    
    info->serv_info = _com_init_serv_info();

    info->rooms = CALLOC(struct_room_list);
    if (!info->rooms)
        errExit("irc_init_info: info rooms failed to calloc.");

    return info;
} /* end irc_init_info() */

/*******************************************************************************
 *  irc_free_info 
 *
 *  frees the struct irc_info.
 *
 *  full_fd_list and cli_list remains NULL since it is handled in the add/remove
 *  functions.
 *
 *  Freeing cli_list and full_fd_list should be handled elsewhere, but
 *  covered here for insurance.
 ******************************************************************************/
void irc_free_info(struct_irc_info *irc_info)
{
    int i;

    /* freeing cli_list if not null */
    if (irc_info->cli_list) {
        /* remove all the indecies */
        for (i=0; i < irc_info->num_clients; ++i) {
            if (_usrLikely(irc_info->cli_list[i] != NULL))
                irc_shutdown_client(irc_info, irc_info->cli_list[i]);
        }

        free(irc_info->cli_list);
    }

    _com_free_serv_info(irc_info->serv_info);

    serv_free_room_list(irc_info->rooms);
    free(irc_info->rooms);
    
    if (irc_info->full_fd_list)
        free(irc_info->full_fd_list);
    free(irc_info);
} /* end irc_free_info() */

/******************************************************************************* 
 * Returns success or failure.
 *
 * Failure means name is taken and client is removed from all lists and can
 * try again.
 ******************************************************************************/
int irc_accept_new_cli(struct_irc_info *irc_info, struct_cli_message *cli_msg,
                       struct_cli_info *cli) 
{
    struct_cli_info *tmp;
    int ret;
    int len;
    

    /* By now, we know it is a logon messages, shit was parsed and checked */
    tmp = serv_find_client(cli_msg->cli_name, cli->sockfd, irc_info->cli_list, 
                           irc_info->num_clients);
    if (tmp == NULL)
        return FAILURE;
    
    if (tmp != NULL && tmp->name == NULL) { 
        /* successful name choice. not taken. */   
        len = strlen(cli_msg->cli_name) + 1; 
        cli->name = CALLOC_ARRAY(char, len);
        if (!cli->name)
            errExit("irc_accept_new_cli: cli name failed to calloc.");

        /* set the new clients name */
        strncpy(cli->name, cli_msg->cli_name, len);

        /* let client know success */
        ret = com_send_logon_result(cli->sockfd, LOGON_SUCCESS);
        if (ret == FAILURE) {
            err_msg("irc_accept_new_cli: failed to send logon response");
            return FAILURE;
        }
        printf("client successfully accepted and responded to\n");
        return SUCCESS;
    }


    /* client name taken or reserved, remove from lists */
    noerr_msg("Client name is taken. Try again client person.");

    /* let client know success */
    com_send_logon_result(cli->sockfd, LOGON_FAILURE);

    return FAILURE;
} /* end irc_accept_new_cli */

int irc_shutdown_client(struct_irc_info *irc_info, struct_cli_info *cli_info)
{
    close(cli_info->sockfd);

    /* remove fd from fd list, decrement totals */
    irc_info->full_fd_list = irc_remove_fd_list(irc_info, cli_info->sockfd);

    /* remove from client list. */
    errno = 0;
    irc_info->cli_list = serv_remove_client(cli_info->name, irc_info->cli_list, 
                                     irc_info->num_clients, cli_info->sockfd);
    if (errno == EINVAL) {
        errno = 0;
        return FAILURE;
    }

    --(irc_info->num_clients);

    serv_free_client(irc_info->rooms, cli_info);

    return SUCCESS;
} /* end irc_shutdown_client */

void irc_take_new_connection(int *nfds, struct_irc_info *irc_info)
{
    unsigned int size = sizeof(struct sockaddr_in);
    struct_cli_info *cli_info;

    cli_info = CALLOC(struct_cli_info);
    if (_usrUnlikely(!cli_info)) {
        errExit("irc_take_new_connection: Malloc failed on serv/cli/irc "
                "info");
    }

    cli_info->sockfd = accept(irc_info->serv_info->sockfd, 
                             (struct sockaddr*) &cli_info->socket_info, 
                             &size);
    if(_usrUnlikely(cli_info->sockfd == FAILURE))
        errExit("irc_take_new_connection: accepting connection failed.\n");

    /* set file descriptor to non-blocking I/O */

    irc_info->full_fd_list = irc_add_fd_list(irc_info, cli_info->sockfd);
    ++(*nfds);

    /* 
     * add new potential client to client list, if following logon
     * message fails.
     */
    irc_info->cli_list = serv_add_client(&cli_info, irc_info->cli_list, 
                                         irc_info->num_clients);
    ++(irc_info->num_clients);
} /* irc_take_new_connection */

/*
 * Incoming client msg format: name | RC_MSG | room | input 
 * Response to client msg format : RC_MSG | client sending | room name | msg 
 *
 * TODO: If time, make a special response for failures instead of ignoring them
 *
 *       client must update history only on recieving a response with the
 *       string that was replyed.
 */
int irc_cli_msg_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg)
{
    int i, j, ret, len;
    char room_name[ROOM_NAME_MAX] = {'\0'};
    char input[MSG_STR_LEN_MAX]   = {'\0'};
    struct_room_info *working_room;
    struct_cli_info *cli;
    
    /* get current room string */
    len = strlen(cli_msg->msg) + 1; // includes '\0'
    if (len > ROOM_NAME_MAX) { /* room name too long, therefore not real. */
        noerr_msg("irc_cli_msg_cmd: Room name provided too long");
        return FAILURE;
    }
    for (i=0; i < len; ++i)
        room_name[i] = cli_msg->msg[i];
    
    /* locate the room */
    working_room = room_find(&irc_info->rooms->rooms, room_name);
    if (!working_room) { /* room does not exist */
        noerr_msg("irc_cli_msg_cmd: Room does not exist");
        return FAILURE;
    }

    /* make sure client is in the room before sending the message. */
    ret = room_check_for_client(cli_msg->cli_name, working_room);
    if (ret == FAILURE) /* client was not in the room. */
        return FAILURE;

    /* get message for the room */
    for (j=0; cli_msg->msg[i] != '\0'; ++i, ++j)
        input[j] = cli_msg->msg[i];
    input[j] = '\0';

    /* add new message to room history */
    ret = room_add_history(&irc_info->rooms->rooms, room_name, input);
    if (ret == FAILURE) {
        noerr_msg("irc_cli_msg_cmd: Issue with room_add_history.");
        return FAILURE;
    }


    /* broadcast to all clients in the room about the message. */
    for (i=0; i < _R_USR_MAX; ++i) {
        if (working_room->room_users[i]) {

            cli = serv_find_client(working_room->room_users[i], 0, 
                                   irc_info->cli_list, irc_info->num_clients);
            if (!cli) {
                errnum_msg(EINVAL, "irc_cli_msg_cmd: Client in room but not"
                                    "in client list");
                return FAILURE;
            }
            com_send_room_message(cli->sockfd, room_name, cli_msg->cli_name, 
                                  input);
        }
    }
    
    return SUCCESS;
} /* end irc_cli_msg_cmd */

/******************************************************************************
 *  irc_cli_join_cmd
 *
 *  Attempts to add a client to the desired room in the message payload.
 *  
 *  msg format: cli_name | RC_JOIN | room name
 *
 *  Returns SUCCESS or FAILURE.
 *
 *  FAILURE state occurs if room attempting to join is full.
 *
 * server to client format: RC_JOIN | room_name | 1/0 | num_users | '\r'
 *
 ******************************************************************************/
int irc_cli_join_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg)
{
    int num_users;
    struct_cli_info *cli;

    /* get clients information */
    cli = serv_find_client(cli_msg->cli_name, 0, irc_info->cli_list,
                           irc_info->num_clients);
    if (!cli) {
        errnum_msg(EINVAL, "irc_cli_join_cmd: Client didnt exist?");
        return FAILURE;
    }

    /* add new room to active room list for client */
    if (serv_add_active_room(cli, cli_msg->msg) == FAILURE) {
        /* user is in max ammount of rooms. */
        com_send_join_result(cli->sockfd, cli_msg->msg, 0,
                             _REPLY_FAILURE);
        return SUCCESS;
    }

    /* add client to the room if there is space */
    num_users = serv_add_to_room(irc_info->rooms, cli_msg->msg, cli->name);
    if (num_users == FAILURE) {
        com_send_join_result(cli->sockfd, cli_msg->msg, 0,
                             _REPLY_FAILURE);
        return FAILURE;
    }

    /* client successfully added to room, let them know. */
    return com_send_join_result(cli->sockfd, cli_msg->msg, num_users,
                                _REPLY_SUCCESS);
} /* end irc_cli_join_cmd */

/* 
 * leave a particular room.
 * client to server msg format: cli_name | RC_LEAVE | room name | \r
 * server to client msg format: RC_LEAVE | result | \r 
 */
int irc_cli_leave_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg)
{
    int ret;
    struct_cli_info *cli;
    
    /* get clients information */
    cli = serv_find_client(cli_msg->cli_name, 0, irc_info->cli_list,
                           irc_info->num_clients);
    if (!cli) {
        errnum_msg(EINVAL, "irc_cli_join_cmd: Client didnt exist?");
        return FAILURE;
    }
    
    /* remove client from the room */
    ret = serv_rem_from_room(irc_info->rooms, cli_msg->msg, cli->name);
    if (ret == FAILURE) {
        /* send message to client about the failure */
        com_send_leave_result(cli->sockfd, cli_msg->msg, _REPLY_FAILURE);
        return FAILURE;
    }
    
    /* remove from clients active room list */
    serv_remove_active_room(cli, cli_msg->msg);

    /* let client know they left the room successfully and can update. */
    return com_send_leave_result(cli->sockfd, cli_msg->msg, _REPLY_SUCCESS);
} /* irc_cli_leave_cmd */

/* 
 * client to server format: cli_name | RC_EXIT | \r
 * server to client format: RC_EXIT | \r
 */
int irc_cli_exit_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg)
{
    struct_cli_info *cli;
    
    /* get clients information */
    cli = serv_find_client(cli_msg->cli_name, 0, irc_info->cli_list,
                           irc_info->num_clients);
    if (!cli) {
        errnum_msg(EINVAL, "irc_cli_join_cmd: Client didnt exist?");
        return FAILURE;
    }

    /* ack client */
    com_send_exit_message(cli->sockfd);

    irc_shutdown_client(irc_info, cli);
    return SUCCESS;
} /* end irc_cli_exit_cmd */


/*
 * client to server format: cli_name | RC_RUL | user_name | \r
 * server to clinet format: RC_RUL | user_name | \r
 *
 * TODO: scope is wonky. was just fastest.
 */
int irc_cli_list_room_users(struct_irc_info *irc_info, 
                            struct_cli_message *cli_msg)
{
    int i;

    struct_room_list *rooms;
    struct_room_info *room_info;
    struct_cli_info *cli;
    
    /* get clients information */
    cli = serv_find_client(cli_msg->cli_name, 0, irc_info->cli_list,
                           irc_info->num_clients);
    if (!cli) {
        errnum_msg(EINVAL, "irc_cli_list_room_users: Client didnt exist?");
        return FAILURE;
    }

    rooms = irc_info->rooms;
    room_info = room_find(&(rooms->rooms), cli_msg->msg);
    if (!room_info) {
        com_send_room_user_message(cli->sockfd, NULL);
        return FAILURE;
    }

    for (i=0; i < _R_USR_MAX; ++i) {
        if (room_info->room_users[i]) {
            com_send_room_user_message(cli->sockfd, room_info->room_users[i]);
            usleep(1);
        }
    }

    return com_send_room_user_message(cli->sockfd, NULL);
} /* end irc_cli_list_room_users */

/*******************************************************************************
 * irc_handle_cli
 *
 * Handles the messages recieved from the client.
 *
 * Returns SUCCESS if operation happened smoothly.
 * Returns FAILURE if anything bad happens.
 ******************************************************************************/
int irc_handle_cli(struct_irc_info *irc_info, struct_cli_info *cli_info) 
{
    uint8_t rx[IO_BUFF] = {'\0'}; 
    struct_cli_message *cli_msg; /* new incoming client message */

    receive_from_client(cli_info->sockfd, rx, IO_BUFF, NO_FLAGS);

    if (rx[0] == '\0') {
        noerr_msg("client disconnecting");
        irc_shutdown_client(irc_info, cli_info);
    }

    /* Parse message */
    cli_msg = com_parse_cli_message(rx);
    if (!cli_msg) {
        /* drop client and move on. likely crashed. */
        err_msg("cli_msg for parse failed.");
        irc_shutdown_client(irc_info, cli_info);
        return FAILURE; 
    }
    
    switch (cli_msg->type) {

    case RC_LOGON:
        /* try and accept new logon */
        if (irc_accept_new_cli(irc_info, cli_msg, cli_info) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }
    break;
    
    case RC_JOIN:
        /* join a specified room. */
        if (irc_cli_join_cmd(irc_info, cli_msg) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }
    break;

    case RC_LEAVE:
        /* leave a specified room */
        if (irc_cli_leave_cmd(irc_info, cli_msg) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }
    break;

    case RC_MSG:
        /* print out message to current room, listed in msg. */
        if (irc_cli_msg_cmd(irc_info, cli_msg) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }
    break;

    case RC_EXIT:
        /* 
         * client is about to shutdown, 
         * feel free to drop them from all the things.
         * msg format: cli_name | RC_EXIT
         */
        if (irc_cli_exit_cmd(irc_info, cli_msg) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }

    break;

    case RC_RUL:
        /* send a list of users in a room */
        if (irc_cli_list_room_users(irc_info, cli_msg) == FAILURE) {
            _com_free_cli_message(cli_msg);
            return FAILURE;
        }
    break;
    default:
        errnum_msg(EINVAL, "Invalid type of msg");
        return FAILURE;
    }

    _com_free_cli_message(cli_msg);
    cli_msg = NULL;
    return SUCCESS;
} /* irc_handle_cli */

/* 
 * Currently Checks to make sure the command is the exit command. 
 * This is where the server debug interface would have come into play for 
 * parsing manual commands.
 */
int irc_check_direct_input(void)
{
    char buff[DIRECT_IO_SIZE] = {'\0'};
    int retBytes = 0;

    /* get message from stdin. newline check, clear buffer, etc. */
    READ_INPUT(STDIN_FILENO, buff, DIRECT_IO_SIZE, retBytes);
    if (retBytes == FAILURE)
        return FAILURE;

    /* remove newline, if it wasnt a newline was invalid command anyway. */
    buff[strlen(buff)-1] = '\0';

    /* make sure the command is /exit */
    if (strcmp(buff, EXIT_CMD) == 0)
        return EXIT_NUM;

    return FAILURE;
} /* end irc_handle_manual_server_command */

void irc_server(void)
{
    /* irc_info w/ pointers for faster access into irc_info */
    struct_irc_info  *irc_info;
    struct_serv_info *irc_serv_info; /* for better access to irc_info member */

    int i;
    int ret; /* for general return values */

    /* select stuff */
    fd_set readfds;
    int nfds;
    struct timeval timeout;
  
    irc_info = irc_init_info();
    if(_usrUnlikely(!irc_info))
        errExit("irc_server: Malloc failed on serv/cli/irc info");
        
    irc_serv_info = irc_info->serv_info;

    init_server_comm(irc_serv_info);

    /* initialize full_fd_list */
    /* adds the server socket file descriptor */
    irc_info->full_fd_list = irc_add_fd_list(irc_info, 
                                             irc_info->serv_info->sockfd);
    /* adds stdin to socket file descriptor */
    irc_info->full_fd_list = irc_add_fd_list(irc_info, STDIN_FILENO);

    /* 
     * start nfds to 1 greater than # of descriptors. Starting garunteed 
     * descriptors are stdin, stdout, stderr, server socket fd.
     */
    irc_info->num_fds = 2; /* the server fd and STDIN */
    nfds = 6;

    for ( ;; ) {
        /* see if any file descriptors are ready for reading */
        irc_fill_fd_set_read(irc_info, &readfds);
        timeout.tv_sec = timeout.tv_usec = 0;

        ret = select(nfds, &readfds, NULL, NULL, &timeout); 
        if (ret < 0)
            errExit("irc_server: Invalid file descriptor given to select.");

        /* TODO: Not sure if you put this at end or start of the loop */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            ret = irc_check_direct_input();
            if (ret == FAILURE)
                continue;
            break;
        }
       
        /* no fd is currently available for reads. loop back for now */
        if (ret == 0)
            continue; 

        /* if server socket is ready for reading with incoming transmittion */
        if (FD_ISSET(irc_serv_info->sockfd, &readfds)) {
            /* accept and init  new client message */
            printf("\nClient connected\n");
            irc_take_new_connection(&nfds, irc_info);
            continue;
        }
        /* else ready read descriptor is not a new connection. */

        /* Find fd that is ready and process them. */
        for (i=0; i < irc_info->num_clients; ++i) {
            ret = FD_ISSET((irc_info->cli_list)[i]->sockfd, &readfds);
            if (ret)
                ret = irc_handle_cli(irc_info, (irc_info->cli_list)[i]);
        }
    } /* end while */

    /* shutdown all active sockets */
    for (i=0; i < irc_info->num_clients; ++i)
        irc_shutdown_client(irc_info, irc_info->cli_list[i]);

    /* shutdown server socket */
    close(irc_info->serv_info->sockfd);

    /* free all the info. */
    irc_free_info(irc_info);
} /* end irc_server */
/******* EOF ********/
