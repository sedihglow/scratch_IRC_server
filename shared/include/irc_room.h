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

/******************************************************************************* 
 * TODO: Client might not be the one checking this since multiple clients can 
 *       try to connet at once. Server needs to let the client know there is 
 *       no room.
 ******************************************************************************/
#define _R_USR_MAX      100 // TODO: See todo above ;)
#define _H_STR_MAX      500 // 500 string history.
#define _H_STR_LEN_MAX  255 // for comparison before additional allocation
#define _R_NAME_LEN_MAX 20  // room name max
#define _R_PW_LEN_MAX   25  // passwords up to 25 characters for private rooms

/* what a room is */
typedef struct room_info {
    char *room_name;
    bool pub_flag;       // is this room public or not?
    char *password;      // if not public. Otherwise dont allocate
    uint32_t num_users;  // TODO: Max num users per channel?
    char **room_users;   // Reallocates in chunks.
    size_t max_name_len; // Given in init, used for freeing room_users[]
    char **history;      // Ring buffer.
    int hist_start;
    int hist_end;
} struct_room_info;
#endif
/**** EOF ****/
