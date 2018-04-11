/*******************************************************************************
 * Filename: irc_client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_client.h"

void free_irc_info(struct_irc_info)
{

}



/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 ******************************************************************************/
void irc_client(void)
{
    struct_irc_info *irc_info; 
  
    serv_info = CALLOC(struct_irc_info);
    if(!struct_irc_info)
        errExit("irc_client: calloc failure serv_info");

    init_client_comm(serv_info);

    /**************************************************************************
     * TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *************************************************************************/

    send_to_server(serv_info->sockfd, "1234", 5, NO_FLAGS); 
} // end irc_client()
/****** EOF ******/
