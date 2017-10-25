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
 ******************************************************************************/
void irc_server(void)
{
    char rx[IO_BUFF] = {'\0'}; /* TODO: May be temp, evaluate on implementation */

    socklen_t          size;
    struct_serv_info   *serv_info;
    struct_cli_info    *cli_info;
    struct sockaddr_in *serv_sock;
  
    serv_info = CALLOC(struct_serv_info);
    cli_info  = CALLOC(struct_cli_info);
    if(_usrUnlikely(!serv_info || !cli_info)){
        errExit("irc_server: Malloc failed on serv/cli info");
    }

    size = sizeof(struct sockaddr_in);

    /* set a pointer for easier access to server socket_info struct */
    serv_sock = &(serv_info->socket_info);

    /* init serv_info  */
    serv_info->domain    = NET_DOMAIN;
    serv_info->pcol      = IP_PROTOCOL;
    serv_info->sock_type = SOCK_TYPE;

    /* dot to binary representation */
    if(inet_pton(NET_DOMAIN, SERV_ADDR, &serv_info->addr) != 1){
        if(errno){
            errExit("irc_server: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_server: Invalid network address string");
    }

    /* init dot representation address */
    serv_info->dot_addr = CALLOC_ARRAY(char, SERV_LEN);
    if(_usrUnlikely(!serv_info->dot_addr)){
        errExit("irc_server: malloc failure dot_addr");
    }

    strncpy(serv_info->dot_addr, SERV_ADDR, strlen(SERV_ADDR));

    /* set socket address information for struct sockAddr_in */
    serv_sock->sin_family      = NET_DOMAIN;
    serv_sock->sin_addr.s_addr = serv_info->addr;

    /* convert port to network order */
    serv_sock->sin_port = serv_info->port = htons(SERV_PORT);

    /* open_connection sets serv_info->sockfd and initiates listen */
    if(_usrUnlikely(open_connection(serv_info) == FAILURE)){
        errExit("irc_server: Initial connection to network failed");
    }

    /* TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *        trying to recieve something and reply, just a single one to start
     *************************************************************************/
  
    cli_info->sockfd = accept(serv_info->sockfd, (void*) &cli_info->socket_info, &size);
    if(_usrUnlikely(cli_info->sockfd == FAILURE)){
        errExit("irc_server: accepting connection failed.\n");
    }

    receive_from_client(cli_info->sockfd, rx, IO_BUFF, NO_FLAGS);

    printf("\nprinting result: %s\n", rx);
    /************************************************************************/
}
