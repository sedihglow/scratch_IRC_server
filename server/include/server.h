/*******************************************************************************
 * Filename: server.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_comm.h"


/* servers information on the client it is communicating with. */
typedef struct client_info{
    char *name;
    int sockfd;
    struct_io_ring *tx;
    struct_io_ring *rx;
    struct sockaddr_in socket_info;
}struct_cli_info;


struct_cli_info** serv_add_client(struct_cli_info **new_cli, 
                                  struct_cli_info **old_list, size_t old_size);

struct_cli_info** serv_remove_client(char *name, struct_cli_info **old_list, 
                                     size_t old_size);

struct_cli_info* serv_find_client(char *find, struct_cli_info **fdlist, 
                                  size_t size);
