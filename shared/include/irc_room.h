/*******************************************************************************
 * Filename: irc_room.h
 *
 * This file is shared with the client and server in development.
 *
 * Written by: James Ross
 ******************************************************************************/
#ifndef _IRC_ROOM_H_
#define _IRC_ROOM_H_

#include "utility_sys.h"

#define _R_USR_MAX      100 
#define _H_STR_MAX      500 // 500 string history.
#define _H_STR_LEN_MAX  255 // for comparison before additional allocation
#define _R_NAME_LEN_MAX 21  // room name max with '\0'
#define _R_PW_LEN_MAX   25  // passwords up to 25 characters for private rooms
#define _R_DFLT_ROOM    "void" // defualt room is the void ~ ~
#define _R_ROOM_MAX     3      // max 3 rooms at a time

/* what a room is */
typedef struct room_info {
    char *room_name;
    bool pub_flag;       // is this room public or not?
    char *password;      // if not public. Otherwise dont allocate
    char **room_users;   // Reallocates in chunks.
    uint32_t num_users;
    size_t max_name_len; // Given in init
    char **history;      // Ring buffer of room history
    int hist_start;
    int hist_end;
} struct_room_info;

struct_room_info* _room_init_info(bool pub, char *pw);
void _room_free_info(struct_room_info *dest);
struct_room_info* _room_get_copy(struct_room_info *src);

int _room_add_user(struct_room_info *room, char *name);
int _room_remove_user(struct_room_info *room, char *cli_name);
#endif
/**** EOF ****/
