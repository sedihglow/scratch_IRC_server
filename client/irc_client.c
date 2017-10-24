/*******************************************************************************
 * Filename: irc_client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_client.h"

/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 ******************************************************************************/
void irc_client(void)
{
    struct_serv_info *serv_info;
    struct sockaddr_in *socket_info;
  
    serv_info = CALLOC(struct_serv_info);
    if(!serv_info){
        errExit("irc_client: malloc failure serv_info");
    }

    /* set a pointer for easier access to socket_info */
    socket_info = &(serv_info->socket_info);

    /* init serv_info  */
    serv_info->domain    = NET_DOMAIN;
    serv_info->pcol      = IP_PROTOCOL;
    serv_info->sock_type = SOCK_TYPE;

    /* dot to binary representation */
    if(inet_pton(NET_DOMAIN, SERV_ADDR, &serv_info->addr) != 1){
        if(errno){
            errExit("irc_client: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errnumExit(EINVAL, "irc_client: Invalid network address string");
    }

    /* init dot representation address */
    serv_info->dot_addr = CALLOC_ARRAY(char, SERV_LEN);
    if(!serv_info->dot_addr){
        errExit("irc_client: malloc failure dot_addr");
    }

    strncpy(serv_info->dot_addr, SERV_ADDR, strlen(SERV_ADDR)+1);

    /* set socket address information for struct sockAddr_in */
    socket_info->sin_family      = NET_DOMAIN;
    socket_info->sin_addr.s_addr = serv_info->addr;

    /* convert port to network order */
    socket_info->sin_port = serv_info->port = htons(SERV_PORT);

    /* connect_to_server() sets serv_info->sockfd */
    if(connect_to_server(serv_info) == FAILURE){
        errExit("irc_client: Initial connection to server failed");
    }

    /* TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *************************************************************************/

    send_to_server(serv_info->sockfd, "1234", 5, NO_FLAGS); 

    /************************************************************************/
}
