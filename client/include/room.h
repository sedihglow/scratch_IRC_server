/******************************************************************************
 * Filename: room.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../shared/include/irc_room.h" // struct_room_info found here.

typedef struct room_state {
    struct_room_info *current_room;
    struct_room_info *next_buff;
    struct_room_info *prev_buff; // TODO: might not need this on implementation 
} struct_room_state;

#define H_STR_LEN_MAX _H_STR_LEN_MAX // Value found in irc_room.h

int switch_to_room(struct_room_state *room_state, struct_room_info *new_room);
/***** EOF *****/
