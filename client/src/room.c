/*******************************************************************************
 * Filename: room.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "room.h"

struct_room_info*  room_init_info(void)
{
    return _room_init_info(true, NULL);
} /* end room_init_info */

void room_free_info(struct_room_info *dest)
{
    _room_free_info(dest);
} /* end room_free_info() */

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
