/*******************************************************************************
 * Filename: room.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "room.h"

struct_room_info* room_find(rbTree *rooms, char *name)
{
    return find_match(rooms, name);
} /* end room_find */


/*
 * new_room is expected to be filled out properly for its state.
 *
 * name will be unallocated i think if its a new user
 *
 */
int room_create(rbTree *rooms, struct_room_info *new_room)
{
    return (give_data(rooms, new_room) ? SUCCESS : FAILURE);
} /* end room_create */


int room_add_history(rbTree *room_list, char *room_name, char *msg)
{
    struct_room_info *room;
    int len;

    room = find_match(room_list, room_name);
    if (!room) { /* room does not exist */
        noerr_msg("invalid room.");
        return FAILURE;
    }
   
    /* add new message to end of the list */
    len = strlen(msg) + 1; 
    if (room->history[room->hist_end] != NULL) {
        free(room->history[room->hist_end]);
    }
    
    /* allocate to appropriate size */
    room->history[room->hist_end] = CALLOC_ARRAY(char, len);

    strncpy(room->history[room->hist_end], msg, len);

    /* increment pointer ring */
    room->hist_end = (room->hist_end + 1) % _H_STR_MAX;
    room->hist_start = (room->hist_start + 1) % _H_STR_MAX;
    
    return SUCCESS;
} /* end room_add_history */

/*******************************************************************************
 *  room_add_user
 *
 *  add a user to a room. 
 *
 *  Returns SUCCESS on victory
 *  Returns EINVAL if room name is too long.
 *  returns FAILURE if room is full.
 ******************************************************************************/
int room_add_user(rbTree *room_list, char *room_name, char *cli_name)
{
    int len;
    int ret;
    struct_room_info *room;
    
    room = find_match(room_list, room_name);
    if (!room) { /* room does not exist */
        room = _room_init_info(true, NULL);

        /* build room info to add to list */
        len = strlen(room_name) + 1;
        if (len > _R_NAME_LEN_MAX) {
            errnum_msg(EINVAL, "room name too long.");
            return EINVAL;
        }

        room->room_name = CALLOC_ARRAY(char, len);
        if (!room->room_name)
            errExit("room_add_user: room->room_name calloc failure.");

        strncpy(room->room_name, room_name, len);

        /* place user in the room list, pointer is handed off to list */
        ret = room_create(room_list, room);
        if (ret == FAILURE)
            errExit("room_add_user: room_create failed.");
    }

    ret = _room_add_user(room, cli_name); 
    if (ret == FAILURE) {
        noerr_msg("room_add_user: Room is full.");
        return FAILURE;
    }

    return SUCCESS;
} /* end room_add_user */

/*******************************************************************************
 *  room_remove_user
 *
 *  Remove a client from the room name.
 *
 * returns SUCCESS
 * returns FAILURE, if room does not exist or if user was not in the room.
 *                  Basically, failure means no change.
 ******************************************************************************/
int room_remove_user(rbTree *room_list, char *room_name, char *cli_name)
{
    struct_room_info *room;
    
    assert(room_name != NULL && cli_name != NULL);

    room = find_match(room_list, room_name);
    if (!room) { /* room does not exist */
        noerr_msg("room_remove_user: room not found, nothing to remove.");
        return FAILURE;
    }
    return _room_remove_user(room, cli_name);
} /* end room_remove_user */

int room_free(rbTree *room_list, char *room_name) 
{
    return (remove_first(room_list, room_name) ? SUCCESS : FAILURE);
} /* end room_free */



/* 
 * TODO NOTE: These functions with these arrays are so awful when scaled. Need real
 * structurs for this stuff if i had more time.
 */
int room_check_for_client(char *cli_name, struct_room_info *room_info)
{
    int i;

    for (i=0; i < _R_USR_MAX; ++i) {
        if (room_info->room_users[i]) {
            if (strcmp(cli_name, room_info->room_users[i]) == 0)
                return SUCCESS;
        }
    }
    return FAILURE;
} /* end room_check_for_client */



/***** EOF *****/
