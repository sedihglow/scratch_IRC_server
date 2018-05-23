/*******************************************************************************
 * Filename: server.h
 *
 * Written by: James Ross
 ******************************************************************************/
 
#include "irc_comm.h" //This was only for io rings i might not use.
#include "room.h"

#define DfLT_CLI_ROOM _R_DFLT_ROOM // found in irc_room.h 
#define MSG_STR_LEN_MAX _H_STR_LEN_MAX  
#define ROOM_NAME_MAX _R_NAME_LEN_MAX  // room name max

/* servers information on the client it is communicating with. */
typedef struct client_info{
    char *name;
    int sockfd;
    struct_io_ring *tx;
    struct_io_ring *rx;
    struct sockaddr_in socket_info;
}struct_cli_info;

typedef struct room_list {
    rbTree rooms;
} struct_room_list;

struct_cli_info** serv_add_client(struct_cli_info **new_cli, 
                                  struct_cli_info **old_list, size_t old_size);

struct_cli_info** serv_remove_client(char *name, struct_cli_info **old_list, 
                                     size_t old_size, int sockfd);

struct_cli_info* serv_find_client(char *find, struct_cli_info **cli_list, 
                                  size_t size);

struct_cli_info* serv_find_fd_client(int fd, struct_cli_info **cli_list, 
                                     size_t size);

/* TODO: Fuctions that call room.c should be here. Will fix scope if time and
 * have irc_server.c call functions out of server.h only.
 */

int serv_add_to_room(struct_room_list *rooms, char *room_name);   // if room does not exist it creates it.
int serv_rem_from_room(); // remove client from room if they are there
int serv_room_msg();      // give a new message to a room.
