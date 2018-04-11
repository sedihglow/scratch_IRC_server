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

#define _R_USR_MAX 100     // TODO: Not sure to keep this just check overflow, also does client need it?
#define _H_STR_MAX 500     // 500 string history.
#define _H_STR_LEN_MAX 255 // for comparison before allocation

/* what a room is */
typedef struct room_info {
    char *room_name;
    bool pub_flag;      // is this room public or not?
    char *password;     // if not public. Otherwise dont allocate
    uint32_t num_users; // TODO: Max num users per channel?
    char **room_users;  // Reallocates in chunks.
    char **history;     // Ring buffer.
    int hist_start;
    int hist_end;
} struct_room_info;
#endif
/**** EOF ****/
