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
        return NULL;
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
 *
 *  sets full_fd_list to null if deallocation happens
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
        return NULL;
    }

    if (!info->full_fd_list && info->num_fds == 0) {
        noerr_msg("num_fd is 0 or full_fd_list was null and empty");
        return NULL;
    }

    if (info->num_fds == 1) {
        free(info->full_fd_list);
        return NULL;
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

/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 *
 *       pull malloc and calloc macros into this beetch.
 ******************************************************************************/
void irc_server(void)
{
    char rx[IO_BUFF] = {'\0'}; /* TODO: temp buffer during dev. */

    struct_irc_info  *irc_info;
    struct_serv_info *irc_serv_info; /* for better access to irc_info member */
    struct_cli_info  *cli_info; /* TODO: Might rename to new_cli on impl. */
    unsigned int size = sizeof(struct sockaddr_in);

    /* select stuff */
    int ret;
    fd_set readfds;
    int nfds;
    struct timeval timeout;
  
    irc_info = irc_init_info();
    cli_info = CALLOC(struct_cli_info);
    if(_usrUnlikely(!irc_info || !cli_info))
        errExit("irc_server: Malloc failed on serv/cli/irc info");

    irc_serv_info = irc_info->serv_info;



    /********************* TODO: START MEM TESTING ******************/
    /* initialize full_fd_list, adds the server socket file descriptor */
    irc_info->full_fd_list = irc_add_fd_list(irc_info, irc_info->serv_info->sockfd);
    irc_info->full_fd_list = irc_remove_fd_list(irc_info, irc_info->serv_info->sockfd);

    irc_free_info(irc_info);
    free(cli_info);
    irc_info = NULL;
    return;
    /******************** END MEM TESTING **************/

    init_server_comm(irc_serv_info);

    /* initialize full_fd_list, adds the server socket file descriptor */
    irc_info->full_fd_list = irc_add_fd_list(irc_info, 
                                             irc_info->serv_info->sockfd);

    /* 
     * start 1 greater than # of descriptors. Starting garunteed descriptors
     * are stdin, stdout, stderr, server socket fd.
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
            cli_info->sockfd = accept(irc_serv_info->sockfd, 
                                     (struct sockaddr*) &cli_info->socket_info, 
                                     &size);
            if(_usrUnlikely(cli_info->sockfd == FAILURE))
                errExit("irc_server: accepting connection failed.\n");

            ++(irc_info->num_fds); /* increment fd count */
            ++nfds;
            
            // receive_from_client(cli_info->sockfd, rx, IO_BUFF, NO_FLAGS);

            /* make sure message is a LOGIN message */
            
            /* Check if the connections name is taken or not. */

            /* If it is not, send a message telling the client. */

            /* If it is, add client to the active client list. */

            /* place new client in the default room */

        }

        printf("\nprinting result: %s\n", rx);
    } // end while
} /* end irc_server */
