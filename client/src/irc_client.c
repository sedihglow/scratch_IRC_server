/*******************************************************************************
 * Filename: irc_client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_client.h"

/******************************************************************************
 * Allocate all members and sub-members of struct to_init.
 *
 * A new struct will be allocated and its pointer will be returned.
 *
 * Returns NULL on error.
 ******************************************************************************/
struct_irc_info* init_irc_info(void)
{
    struct_irc_info *init;
    init = CALLOC(struct_irc_info);
    if(!init)
        return NULL;

    init->client    = cli_init_info();
    init->serv_info = com_init_serv_info();
    init->tx        = com_init_io_buff();
    init->rx        = com_init_io_buff();

    return init;
}


void free_irc_info(struct_irc_info **dest)
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
  

    irc_info = init_irc_info();
    if (!irc_info)
        errExit("irc_client: init_irc_info failed.");


    //init_client_comm(serv_info);

    /**************************************************************************
     * TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *************************************************************************/

    //send_to_server(serv_info->sockfd, "1234", 5, NO_FLAGS); 
} // end irc_client()
/****** EOF ******/
