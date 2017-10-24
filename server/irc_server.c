/*******************************************************************************
 * Filename: irc_server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_server.h"

void irc_server(void)
{
    struct_serv_info *serv_info;
    struct sockaddr_in *serv_sock;
  
    serv_info = (struct_serv_info*)malloc(sizeof(struct_serv_info));
    if(!serv_info){
        errExit("irc_client: malloc failure serv_info");
    }

    /* set a pointer for easier access to serv_sock */
    serv_sock = &(serv_info->socket_info);

    /* init serv_info  */
    serv_info->domain = NET_DOMAIN;
    serv_info->port   = SERV_PORT;

    /* dot to binary representation */
    if(inet_pton(serv_info->domain, SERV_ADDR, &serv_info->addr) != 1){
        if(errno){
            errExit("irc_client: inet_pton failed to convert IP to binary "
                    "network order");
        }
        errno = EINVAL;
        errExit("irc_client: Invalid network address string");
    }

    /* init dot representation address */
    serv_info->dot_addr = (char*)malloc(sizeof(char) * (SERV_LEN+1));
    if(!serv_info->dot_addr){
        errExit("irc_client: malloc failure dot_addr");
    }

    strncpy(serv_info->dot_addr, SERV_ADDR, strlen(SERV_ADDR));

    /* set socket address information for struct sockAddr_in */
    serv_sock->sin_family      = NET_DOMAIN;
    serv_sock->sin_addr.s_addr = serv_info->addr;
    serv_sock->sin_port        = htons(SERV_PORT); /* conv net byte order */

    /* open_connection sets serv_info->sockfd and initiates listen */
    if(open_connection(serv_info) != SUCCESS){
        errExit("irc_client: Initial connection to server failed");
    }

    /* TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *        trying to recieve something and reply.
     *************************************************************************/
    
     

    /************************************************************************/
}
