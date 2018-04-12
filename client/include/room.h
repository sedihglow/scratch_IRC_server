/******************************************************************************
 * Filename: room.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_room.h" // struct_room_info found here.

/* NOTE: Definitions starting with _ are found in shared folder irc_room.h */

typedef struct room_state {
    struct_room_info *current_room;
    struct_room_info *next_buff;
    struct_room_info *prev_buff; // TODO: might not need this on implementation 
} struct_room_state;

struct_room_state* room_init_state(size_t user_name_size);
struct_room_info*  room_init_info(size_t user_name_size);
int room_switch(struct_room_state *room_state, struct_room_info *new_room);
/***** EOF *****/
