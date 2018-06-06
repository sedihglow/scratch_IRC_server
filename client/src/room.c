/*******************************************************************************
 * Filename: room.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "room.h"

struct_room_state* room_init_state(void)
{
    struct_room_state *init;

    init = CALLOC(struct_room_state);
    if (!init) {
        err_msg("room_init_state: calloc failure - struct_room_state");
        return NULL;
    }
  
    /***************************************************************************
     * default initialization has no public/password set.
     *
     * TODO:
     *      Might change this depending on implementation of shit.
     **************************************************************************/
    init->current_room = _room_init_info(true, NULL);
    init->next_buff    = _room_init_info(true, NULL);
    init->prev_buff    = _room_init_info(true, NULL);

    return init;
} /* end room_init_state() */

struct_room_info*  room_init_info(void)
{
    return _room_init_info(true, NULL);
} /* end room_init_info */

void room_free_state(struct_room_state *dest)
{
    _room_free_info(dest->current_room);
    _room_free_info(dest->next_buff);
    _room_free_info(dest->prev_buff);
} /* end room_free_state() */

void room_free_info(struct_room_info *dest)
{
    _room_free_info(dest);
} /* end room_free_info() */

/******************************************************************************* 
 * TODO: Encrypt the password, decrypt it here for comparison.
 *
 * Compares the password in current_room with the password passed in 
 ******************************************************************************/
static int check_room_pw(struct_room_info *current_room, char *pw_in) 
{
    return 0;
} /* end check_room_pw */

int room_add_to_history(struct_room_info *room, char *msg) 
{
    size_t len;

    /* add new message to end of the list */
    len = strlen(msg) + 1; 
    if (room->history[room->hist_end] != NULL) {
        free(room->history[room->hist_end]);
        room->history[room->hist_end] = NULL;

        room->hist_start = (room->hist_start + 1) % _H_STR_MAX;
    }
    
    /* allocate to appropriate size */
    room->history[room->hist_end] = CALLOC_ARRAY(char, len);

    strncpy(room->history[room->hist_end], msg, len);

    /* increment pointer ring */
    room->hist_end = (room->hist_end + 1) % _H_STR_MAX;
    
    return SUCCESS;
} /* end room_add_to_history */
/**** EOF ****/
