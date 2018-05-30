#include "../include/irc_comm.h"

struct_serv_info* _com_init_serv_info(void)
{
    struct_serv_info *init;

    init = CALLOC(struct_serv_info);
    if (!init) {
        err_msg("_com_init_serv_info: calloc failure - struct_serv_info");
        return NULL;
    }
    
    /* init dot representation address */
    init->dot_addr = CALLOC_ARRAY(char, _COM_SERV_LEN);
    if (!init->dot_addr) {
        err_msg("_com_init_serv_info: calloc failure - dot_addr");
        return NULL;
    }

    init->socket_info = CALLOC(struct sockaddr_in);
    if (!init->socket_info) {
        err_msg("_com_init_serv_info: calloc failure - socket_info");
        return NULL;
    }
    
    /* non allocated initializations */
    init->addr      = 0;
    init->port      = 0;
    init->domain    = 0;
    init->sock_type = 0;
    init->pcol      = 0;
    init->sockfd    = 0;

    return init;
} /* end _com_init_serv_info */

struct_io_ring* _com_init_io_ring() 
{
    struct_io_ring *init;

    init = CALLOC(struct_io_ring);
    if (!init) {
        err_msg("_com_init_io_buff: calloc failure - struct_io_ring");
        return NULL;
    }

    init->buff = CALLOC_ARRAY(char, IO_RING_SIZE);
    if (!init->buff) {
        err_msg("_com_init_io_buff: calloc failure - buff");
        return NULL;
    }
    
    /* non allocated initializations */
    init->start = 0;
    init->end   = 0;
    
    return init;
} /* end com_init_io_buff */


void _com_free_serv_info(struct_serv_info *dest)
{
    FREE_ALL(dest->dot_addr, dest->socket_info, dest);
} /* end _com_free_serv_info() */

void _com_free_io_ring(struct_io_ring *dest)
{
    free(dest->buff);
    free(dest);
} /* end _com_free_io_ring() */

void _com_free_cli_message(struct_cli_message *rem)
{
    FREE_ALL(rem->cli_name, rem->msg, rem);
} /* end _com_free_cli_message */

void _com_free_serv_message(struct_serv_message *rem)
{
    FREE_ALL(rem->msg, rem);
} /* end _com_free_serv_message */


/*** EOF ***/
