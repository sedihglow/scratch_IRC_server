/*******************************************************************************
 * Filename: room.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "room.h"


struct_room_info* room_init_info(size_t user_name_size)
{
    struct_room_info *init;
    int i;

    init = CALLOC(struct_room_info);
    if (!init) {
        err_msg("room_init_info: calloc failure - struct_roomt_info.");
        return NULL;
    }
    
    init->room_name = CALLOC_ARRAY(char, _R_NAME_LEN_MAX);
    if (!init->room_name) {
        err_msg("room_init_info:: calloc failure - room_name.");
        return NULL;
    }

    init->password = CALLOC_ARRAY(char, _R_PW_LEN_MAX);
    if (!init->password) {
        err_msg("room_init_info:: calloc failure - password.");
        return NULL;
    }

    init->room_users = CALLOC_ARRAY(char*, _R_USR_MAX);
    if (!init->room_users) {
        err_msg("room_init_info:: calloc failure - room_users.");
        return NULL;
    }

    for (i=0; i < _R_USR_MAX; ++i) {
        init->room_users[i] = CALLOC_ARRAY(char, user_name_size);
        if (!init->room_users[i]) {
            err_msg("room_init_info:: calloc failure - room_users.");
            return NULL;
        }
    }

    init->history = CALLOC_ARRAY(char*, _H_STR_MAX);
    if (!init->history) {
        err_msg("room_init_info:: calloc failure - history.");
        return NULL;
    }

    for (i=0; i < _H_STR_MAX; ++i) {
        init->history[i] = CALLOC_ARRAY(char, _H_STR_LEN_MAX);
        if (!init->history[i]) {
            err_msg("room_init_info:: calloc failure - history index %d.", i);
            return NULL;
        }
    }

    /* non-allocated initializations */
    init->pub_flag   = 0;
    init->num_users  = 0;
    init->hist_start = 0;
    init->hist_end   = 0;
    init->max_name_len = user_name_size;

    return init;
}

struct_room_state* room_init_state(size_t user_name_size)
{
    struct_room_state *init;

    init = CALLOC(struct_room_state);
    if (!init) {
        err_msg("room_init_state: calloc failure - struct_room_state");
        return NULL;
    }
    
    init->current_room = room_init_info(user_name_size);
    init->next_buff    = room_init_info(user_name_size);
    init->prev_buff    = room_init_info(user_name_size);

    return init;
}


void room_free_state(struct_room_state *dest)
{
    room_free_info(dest->current_room);
    room_free_info(dest->next_buff);
    room_free_info(dest->prev_buff);
}

void room_free_info(struct_room_info *dest)
{
    unsigned int i;

    /* free room user indecies */
    for (i=0; i < dest->max_name_len; ++i)
        free(dest->room_users[i]);

    /* free history indecies */
    for (i=0; i < _H_STR_LEN_MAX ; ++i)
        free(dest->history[i]);
    
    FREE_ALL(dest->room_name, dest->password, dest->room_users, dest->history);
}

/******************************************************************************* 
 * TODO: Encrypt the password, decrypt it here for comparison.
 *
 * Compares the password in current_room with the password passed in 
 ******************************************************************************/
static int check_room_pw(struct_room_info *current_room, char *pw_in) 
{
    return 0;
}

int room_switch(struct_room_state *room_state, struct_room_info *new_room) 
{
    return 0;
}
/**** EOF ****/
