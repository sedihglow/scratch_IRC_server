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
    char *active_rooms[_R_ROOM_MAX];
    int sockfd;
    struct sockaddr_in socket_info;
}struct_cli_info;

typedef struct room_list {
    rbTree rooms;
} struct_room_list;

struct_cli_info** serv_add_client(struct_cli_info **new_cli, 
                                  struct_cli_info **old_list, size_t old_size);

struct_cli_info** serv_remove_client(char *name, struct_cli_info **old_list, 
                                     size_t old_size, int sockfd);

struct_cli_info* serv_find_client(char *find, int fd, struct_cli_info **cli_list, 
                                  size_t size);

struct_cli_info* serv_find_fd_client(int fd, struct_cli_info **cli_list, 
                                     size_t size);

void serv_remove_active_room(struct_cli_info *cli, char *room_name);
int serv_add_active_room(struct_cli_info *cli, char *room_name);

void serv_free_client(struct_room_list *rooms, struct_cli_info *cli);

int serv_add_to_room(struct_room_list *rooms, char *room_name, char *cli_name);
int serv_rem_from_room(struct_room_list *rooms, char *room_name, char *cli_name);
void serv_free_room_list(struct_room_list *room_list);
