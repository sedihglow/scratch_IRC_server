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
    
    return init;
} /* end _com_init_serv_info */

void _com_free_serv_info(struct_serv_info *dest)
{
    FREE_ALL(dest->dot_addr, dest->socket_info, dest);
} /* end _com_free_serv_info() */

void _com_free_cli_message(struct_cli_message *rem)
{
    FREE_ALL(rem->cli_name, rem->msg, rem);
} /* end _com_free_cli_message */

void _com_free_serv_message(struct_serv_message *rem)
{
    FREE_ALL(rem->msg, rem);
} /* end _com_free_serv_message */


/*** EOF ***/
