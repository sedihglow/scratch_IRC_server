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
    init->tx        = com_init_io_ring();
    init->rx        = com_init_io_ring();

    return init;
}

/* TODO: Note, not a ** so dest does not get set to NULL, memory is just freed, 
 * .... assuming i know what the fuck pointers do. ;) aka test the shit
 */
void free_irc_info(struct_irc_info *dest)
{
    cli_free_info(dest->client);
    com_free_serv_info(dest->serv_info);
    com_free_io_ring(dest->tx);
    com_free_io_ring(dest->rx);
    free(dest);
    dest = NULL;
}

/*******************************************************************************
 * TODO: Cleaning up on error before exiting not implemented yet. Mallocs not
 *       freed on error yet. Waiting until i know how i want to handle them
 *       since in user space.
 ******************************************************************************/
void irc_client(void)
{
    struct_irc_info *irc_info; 
    bool exit = false;

    irc_info = init_irc_info();
    if (!irc_info)
        errExit("irc_client: init_irc_info failed.");

    /* TODO: Figure out exit flag after you figure out getting input from user */
    while (!exit) {

    }

    irc_free_info(irc_info);

    /**************************************************************************
     * TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *************************************************************************/
} // end irc_client()
/****** EOF ******/
