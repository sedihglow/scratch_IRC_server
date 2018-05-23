/*******************************************************************************
 * Filename: irc_room.c
 *
 * Functions that are shared between client and server involving rooms.
 * 
 * Written by: James Ross
 ******************************************************************************/


#include "irc_room.h"

/******************************************************************************
 * allocate a room struct and return it.
 *
 * NOTE: If pub is false, or pw is NULL, then the password member will not be 
 * allocated.
 *
 * Does not alloacte room_users, left to client or server to build that data
 * when required.
 *
 * Gives room_name the _R_DFLT_ROOM  name.
 *
 *
 * TODO: CLean up allocations on failure.
 *       Passworded rooms will be put in if time, set up for it.
 ******************************************************************************/
struct_room_info* _room_init_info(bool pub, char *pw)
{
    struct_room_info *init;
    size_t len;

    if (pub == false && pw == NULL) {
        err_msg("room_init_info: pub flag is private but no pw provided..");
        return NULL;
    }

    init = CALLOC(struct_room_info);
    if (!init) {
        err_msg("room_init_info: calloc failure - struct_roomt_info.");
        return NULL;
    }
 
    /* set password if room is private, maybe do this outside of funct? */
    if (pub == false) {
        len = strlen(pw)+1;
        init->password = CALLOC_ARRAY(char, len);
        if (!init->password) {
            err_msg("room_init_info: calloc failure - password.");
            return NULL;
        }

        strncpy(init->password, pw, len);
        init->pub_flag = pub;
    } else {
        init->pub_flag = true;
    }

    /* TODO: basically static. Not enough time for proper data structures  D: */
    init->room_users = CALLOC_ARRAY(char*, _R_USR_MAX);
    if (!init->room_users) {
        err_msg("room_init_info: calloc failure - room_users.");
        return NULL;
    }

    /* note history is a ring buffer, felt it okay to allocate it all here
     * for now.
     *
     * Allocate entire ring buffer, without its members
     */
    init->history = CALLOC_ARRAY(char*, _H_STR_MAX);
    if (!init->history) {
        err_msg("room_init_info: calloc failure - history.");
        return NULL;
    }

    init->max_name_len = _R_NAME_LEN_MAX;
    init->hist_start = _H_STR_MAX-1;

    return init;
} /* end _room_init_info() */


/*******************************************************************************
 *  _room_free_info
 *  
 *  Frees the struct_room_info structure.
 *
 *  ***NOTE: Does NOT set passed dest to NULL in calling function
 ******************************************************************************/
void _room_free_info(struct_room_info *dest)
{
    unsigned int i;

    /* free room user indecies */
    if (dest->room_users) {
        for (i=0; dest->room_users[i] != NULL; ++i) {
            if (dest->room_users[i])
                free(dest->room_users[i]);
        }
    }

    /* free history indecies */
    for (i=0; i < _H_STR_MAX ; ++i) {
        if (dest->history[i]) 
            free(dest->history[i]);
    }
    
    FREE_ALL(dest->room_name, dest->password, dest->room_users, dest->history,
             dest);
} /* end _room_free_info() */

/*******************************************************************************
 * _room_get_copy
 *
 * Creates a new struct_room_info and copys all contents of src into the new
 * structure.
 *
 * Returns a pointer to the copied data on success.
 * Returns NULL on error.
 *   - Errors include if something in the src struct is not allocated.
 *   - Exception with password string, which can be unallocated if pub_flag is
 *     false.
 *
 * NOTE:
 * By the time something gets here, it should have already made any alterations
 * to the existing room if need be per programer application use.
 ******************************************************************************/
struct_room_info* _room_get_copy(struct_room_info *src)
{
    struct_room_info *copy;
    size_t len;
    uint32_t i;
    
    copy = _room_init_info(src->pub_flag, src->password);
    if (!copy) {
        err_msg("_room_get_copy: Failed to allocate copy");
        goto clean_exit;
    }

    /* create the required number of users in the room */
    copy->room_users = CALLOC_ARRAY(char*, src->num_users);
    if (!copy->room_users) {
        err_msg("_room_get_copy: calloc failure - room_users.");
        goto clean_exit;
    }

    /* copy room user names into the list of room users  */
    for (i=0; i < src->num_users; ++i) {
        len = strlen(src->room_users[i]) + 1;
        copy->room_users[i] = CALLOC_ARRAY(char, len);
        if (!copy->room_users[i]) {
            err_msg("_room_get_copy: calloc failure - room_users index %d.", i);
            goto clean_exit;
        }
        
        strncpy(copy->room_users[i], src->room_users[i], len);
    }

    /* copy history */
    for (i=0; i < _H_STR_MAX; ++i) {
        len = strlen(src->history[i]) + 1;
        copy->history[i] = CALLOC_ARRAY(char, len);
        if (!copy->history[i]) {
            err_msg("_room_get_copy: calloc failure - history index %d.", i);
            goto clean_exit;
        }

        strncpy(copy->history[i], src->history[i], len);
    }

    /* non pointer data */
    copy->pub_flag     = src->pub_flag;
    copy->num_users    = src->num_users;
    copy->max_name_len = src->max_name_len;
    copy->hist_start   = src->hist_start;
    copy->hist_end     = src->hist_end;

    return copy;

/* deallocate any allocated data during copy on error, returning null */
clean_exit: 
    _room_free_info(copy);
    return NULL;
} /* end _room_get_copy() */


/*******************************************************************************
 *  _room_add_user
 *
 *  If the user is already in the room, they will be removed and re-added.
 *
 *  errExits on allocation failure
 *
 *  otherwise, returns success.
 ******************************************************************************/
int _room_add_user(struct_room_info *room, char *name)
{
    int i;
    int len = strlen(name) + 1;

    if (room->num_users == _R_USR_MAX)
        return FAILURE;

    /* 
     * make sure user is not already in the room. If so, remove and add them 
     * Client should just end up doing nothing or clearing everything and act
     * like they just joined.
     */
    _room_remove_user(room, name);

    for (i=0; i < _R_USR_MAX; ++i) {
        if (room->room_users[i] == NULL) {

            room->room_users[i] = CALLOC_ARRAY(char, len);
            if (!room->room_users[i]) 
                errExit("_room_add_user: calloc failure on room user.");

            strncpy(room->room_users[i], name, len);
            ++(room->num_users);
            return SUCCESS;
        }
    }

    return FAILURE;
} /* end _room_add_user */


/*******************************************************************************
 *  _room_remove_user
 *
 *  Removes a user from the provided struct room info.
 *
 *  If user was not found in the room, failure is returned.
 *  Otherwise, success
 ******************************************************************************/
int _room_remove_user(struct_room_info *room, char *cli_name)
{
    int i;

    if (room->num_users == 0)
        return SUCCESS;

    for (i=0; i < _R_USR_MAX; ++i) {
        if (room->room_users[i] != NULL) {
            if (strcmp(room->room_users[i], cli_name) == 0) {
                FREE_ALL(room->room_users[i]);
                room->room_users[i] = NULL;
                --(room->num_users);
                return SUCCESS;
            }
        }
    }

    return FAILURE;

} /* end room_remove_user */
/***** EOF ****/
