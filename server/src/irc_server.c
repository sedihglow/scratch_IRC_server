/*******************************************************************************
 * Filename: irc_server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_server.h"

/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 *
 *       pull malloc and calloc macros into this beetch.
 ******************************************************************************/
void irc_server(void)
{
    char rx[IO_BUFF] = {'\0'}; /* TODO: May be temp, evaluate on implementation */

    socklen_t size = sizeof(struct sockaddr_in);
    struct_serv_info   *serv_info;
    struct_cli_info    *cli_info;
  
    serv_info = _com_init_serv_info();

    cli_info  = CALLOC(struct_cli_info);
    if(_usrUnlikely(!serv_info || !cli_info)){
        errExit("irc_server: Malloc failed on serv/cli info");
    }

    init_server_comm(serv_info);

    /* TODO: Pretty sure while loop goes here */
    cli_info->sockfd = accept(serv_info->sockfd, 
                             (struct sockaddr*) &cli_info->socket_info, &size);
    if(_usrUnlikely(cli_info->sockfd == FAILURE)) {
        errExit("irc_server: accepting connection failed.\n");
    }

    receive_from_client(cli_info->sockfd, rx, IO_BUFF, NO_FLAGS);

    printf("\nprinting result: %s\n", rx);
    /************************************************************************/
}
