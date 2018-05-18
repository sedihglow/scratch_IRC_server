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

    if (!irc_info || !irc_info->full_fd_list || !readfds) {
        errnum_msg(EINVAL, "fill_fd_set_read: info, fdlist or readfs was NULL.");
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
        return new_list;
    }
    
    old_list = info->full_fd_list;
    old_size = info->num_fds;
    new_list = CALLOC_ARRAY(int, info->num_fds + 1);

    for (i=0; i < old_size; ++i)
        new_list[i] = old_list[i];

    new_list[i] = newfd;

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

    if (irc_info->cli_list) {  
        for (i=0; i < irc_info->num_clients; ++i) {
            if (_usrLikely(irc_info->cli_list[i] != NULL))
                free(irc_info->cli_list[i]);
        }
        free(irc_info->cli_list);
    }

    _com_free_serv_info(irc_info->serv_info);
    
    if (irc_info->full_fd_list)
        free(irc_info->full_fd_list);
    free(irc_info);
} /* end irc_free_info() */


/* Returns success or failure.
 *
 * Failure means name is taken and client is removed from all lists and can
 * try again.
 *
 *
 * TODO: Not a finished function.
 * 
 * TODO: Might want to write a function that compares the file descriptor in
 *       case multiple people are the DELETE_CLI string at once, trying
 *       to access at the same time when the username is taken :/
 *
 *       Maybe send cli_info into serv_remove_client
 */
int irc_accept_new_cli(struct_irc_info *irc_info, struct_cli_message *cli_msg,
                       struct_cli_info *cli) 
{
    struct_cli_info *tmp;
    void ** ret;

    /* By now, we know it is a logon messages, shit was parsed and checked */
    tmp = serv_find_client(cli_msg->cli_name, irc_info->cli_list, 
                           irc_info->num_fds);

    if (tmp || strcmp(cli_msg->cli_name, DELETE_CLI) == 0) { 
        /* client name taken or reserved, remove from lists */
        noerr_msg("Client name is taken. Try again client person.");

        /* TODO: Realize the hoops i am jumping now. And it brings tears. */
        cli->name = CALLOC_ARRAY(char, sizeof(DELETE_CLI)+1);
        if (!cli->name)
            errExit("irc_accept_new_cli: calloc failure. Download more ram.");

        strcpy(cli->name, DELETE_CLI);
        /* end hoops */

        /* remove fd from fd list, decrement totals */
        irc_info->full_fd_list = irc_remove_fd_list(irc_info, cli->sockfd);
        --(irc_info->full_fd_list);

        /* remove from client list. Had to utilize dumb string because scope */
        ret = (void**)serv_remove_client(DELETE_CLI, irc_info->cli_list, 
                                         irc_info->num_clients);
        if (ret != NULL)
            irc_info->cli_list = (struct_cli_info**)ret;

        --(irc_info->num_clients);

        return FAILURE;
    }
    /* set the new clients name */
    strcpy(cli->name, cli_msg->cli_name);

    /* place client in default room */
    serv_add_to_room(irc_info->rooms, DfLT_CLI_ROOM);

    return SUCCESS;
} /* end irc_accept_new_cli */

void irc_take_new_connection(int *nfds, struct_irc_info *irc_info)
{
    unsigned int size = sizeof(struct sockaddr_in);
    char rx; /* first revieve should be 0 */
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

    irc_info->full_fd_list = irc_add_fd_list(irc_info, cli_info->sockfd);
    ++(irc_info->num_fds); /* increment fd count */
    ++(*nfds);

    /* make sure buffer is read and empty, not sure if accept does this*/
    receive_from_client(cli_info->sockfd, &rx, IO_BUFF, NO_FLAGS);
   
    /* add new potential client to client list, if following logon
     * message fails, it will be deallocated with the shit dealloc
     * realloc structure because no time to make legit data
     * structures. /wrists.
     */
    irc_info->cli_list = serv_add_client(&cli_info, irc_info->cli_list, 
                                         irc_info->num_clients);
    ++(irc_info->num_clients);
} /* irc_take_new_connection */

/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 *
 *       pull malloc and calloc macros into this beetch.
 ******************************************************************************/
void irc_server(void)
{
    char rx[IO_BUFF] = {'\0'}; /* TODO: likely temp buffer during dev. */

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

    /* initialize full_fd_list, adds the server socket file descriptor */
    irc_info->full_fd_list = irc_add_fd_list(irc_info, 
                                             irc_info->serv_info->sockfd);

    /* 
     * start nfds to 1 greater than # of descriptors. Starting garunteed 
     * descriptors are stdin, stdout, stderr, server socket fd.
     */
    irc_info->num_fds = 1; /* the server fd */
    nfds = 5;
    while (true) { /* TODO: Tempararily true until signal handling happens */

        /* see if any file descriptors are ready for reading */
        irc_fill_fd_set_read(irc_info, &readfds);
        timeout.tv_sec = timeout.tv_usec = 0;
        ret = select(nfds, &readfds, NULL, NULL, &timeout); 
        if (ret < 0)
            errExit("irc_server: Invalid file descriptor given to select.");
       
        /* no fd is currently available for reads. loop back for now */
        if (ret == 0)
            continue; 

        /* if server socket is ready for reading with incoming transmittion */
        if (FD_ISSET(irc_serv_info->sockfd, &readfds)) {
            /* accept and init  new client message */
            irc_take_new_connection(&nfds, irc_info);
            continue;
        }
        /* else ready read descriptor is not a new connection. */

        /* Find fd that is ready and process them. */
        for (i=0; i < irc_info->num_clients; ++i) {
            ret = FD_ISSET((irc_info->cli_list)[i]->sockfd, &readfds);
            if (ret)
                irc_handle_cli(irc_info, (irc_info->cli_list)[i]);
        }
    } // end while
} /* end irc_server */


int irc_cli_msg_cmd(struct_cli_message *cli_msg)
{
    int i, len;
    char room_name[_R_NAME_MAX] = {\'0'};
    char input[MSG_STR_LEN_MAX] = {'\0'};
    
    /* get current room string */
    len = strlen(&(cli_msg->msg));
    if (len > _R_NAME_MAX) {
        noerr_msg("irc_cli_msg_cmd: Room name provided too long");
        return EINVAL;
    }
    for (i=0; i < len; ++i) {
        room_name[i] = cli->msg[i];

    /* get message for the room */
    for (; i < MSG_STR_LEN_MAX && cli_msg->msg[i] != '\r'; ++i)
        input[i] = cli_msg->msg[i];

    /* add new message to room history */
    room_add_history(irc_info->rooms->rooms, room_name, input);
    

    /* TODO: Send new history information to all clients in the room.
     *       place in buffer and make select read writes as well?
     *       send then.
     */
} /* end irc_cli_msg_cmd */


int irc_handle_cli(struct_irc_info *irc_info, struct_cli_info *cli_info) 
{
    char rx[IO_BUFF] = {'\0'}; 
    struct_cli_message *cli_msg; /* new incoming client message */
    int i, len;

    receive_from_client(cli_info->sockfd, rx, IO_BUFF, NO_FLAGS);

    /* Parse message */
    cli_msg = com_parse_cli_message(rx);
    if (!cli_msg) {
        err_msg("cli_msg for parse failed.");
        return 0; /* drop and move on. */
    }
    
    /*
     *  Name must be a valid client to be here already, error if cant find cli 
     *  w/ response.
     */
    switch (cli_msg->type) {

    case RC_LOGON:
        /* try and accept new logon */
        if (!(irc_accept_new_cli(irc_info, cli_msg, cli_info))) {
            /* issue with client name */
            /* TODO: Send message to current client telling them they suck */
            return FAILURE;
        }

        /* client successfully joined, named and placed in the void */
        /* TODO: Send confirmation to client */
         
    break;
    case RC_MSG:
        /*
         * print out message to current room, listed in msg.
         * msg format RC_MSG: name | type | current room | input 
         */

                                    
    /* broadcast to all clients in the room about the message.
     * msg format: RC_MSG | client sending | room name | msg 
     */

    break;
    case RC_WHO:
    /* from /who <name>
     * msg format: cli_name | RC_WHO | name to check 
     */

    /* Reply to client with a true or false statment.
     * msg format: RC_WHO | name to check | online or offline.
     */

    break;
    case RC_JOIN:
    /* join a specified room. 
     * msg format: cli_name | RC_JOIN | room name
     */

    /* once added to the room, let all clients in that room know this person
     * exists.
     *
     * Client only adds someone to the room after getting a response.
     */

    break;
    case RC_EXIT:
    
    /* client is about to shutdown, feel free to drop them from all the things.
     * msg format: cli_name | RC_EXIT
     */
    

    break;
    case RC_VOID:
    /* remove user from all rooms and place them in the void. */

    /* let clients who shared a room know this user no longer exists in the 
     * room.
     *
     * Same function that would be used to leave a room.
     */

    break;


    case RC_RL:
    /* list all publicly created rooms.
     * msg format: cli_name | RC_RL | */

    /* Server must then send the name of every room to the client for display.
     *
     * lame.
     *
     * have the return msg be :
     *
     * PRINT | room_name\n 
     */

    break;

    case RC_R_MSG:
    /* Send a message to a particular room.
     * msg format: cli_name | RC_R_MSG | room name | msg
     */

    break;


    case RC_LEAVE:
    /* leave a particular room.
     * msg format: cli_name | RC_LEAVE | room name
     */
    break;



    case RC_PM: // if time should be quick when time comes.
    break;
    case RC_PR: // if time should be quick
    break;

    case RC_LOGOUT: // implemented if/when persistent client info saved.
    break;
    case RC_INV:   // invite to room can still happen if time.
    break;


    /* TODO: Friends list is local on client untill i have persistent client
     *       login w/ pw credentials implemented */
    case RC_FA:

    break;
    case RC_FL:

    case RC_FR:
    break;

    break;
    case RC_BL:

    break;
    case RC_BA:

    break;
    case RC_BR:
    /* end TODO */

    case RC_ERR:
    break;

    default:
        errnum_msg(EINVAL, "Invalid type of msg");

    }



    _com_free_cli_message(cli_msg);
    cli_msg = NULL;
} /* irc_handle_cli */

