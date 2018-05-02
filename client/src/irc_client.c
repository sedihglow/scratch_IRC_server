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
    if (!init->client)
        return NULL;
    init->serv_info = com_init_serv_info();
    init->tx        = com_init_io_ring();
    init->rx        = com_init_io_ring();
    return init;
} /* end init_irc_info() */

void irc_free_info(struct_irc_info *dest)
{
    cli_free_info(dest->client);
    com_free_serv_info(dest->serv_info);
    com_free_io_ring(dest->tx);
    com_free_io_ring(dest->rx);
    free(dest);
} /* end irc_free_info() */

void irc_client(void)
{
    struct_irc_info *irc_info; 
    int ret;
    int current_cmd;
    char *input; /* input is likely to be handled in a thread. */
    bool debug_on = false; /* definition is debug.h, should be an argv. */

    irc_info = init_irc_info();
    if (!irc_info)
        errExit("irc_client: init_irc_info failed.");

    init_client_comm(irc_info->serv_info);

#if 0
    /* TODO: Figure out exit flag after you figure out getting input from user */
    bool exit = true; // temp here while testing shit
    while (!exit) {
        /* Start debug cmd block, NOTE: Parsing might be handled in a thread */
        if (debug_on) {
            ret = strncmp(input, "/d ", 3);
            if (ret == 0) {
                current_cmd = find_debug_cmd(input);
            }
        } /* End debug cmd block */

    }
#endif

    ret = send_to_server(irc_info->serv_info->sockfd, "void", sizeof("void"), 0);

    irc_free_info(irc_info);

    /**************************************************************************
     * TODO: Sending message through port for initial testing. Implementation
     *        for messages will change and likely include signals/processes or
     *        exceptions.
     *************************************************************************/
} /* end irc_client() */
/****** EOF ******/
