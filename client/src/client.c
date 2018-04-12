/*******************************************************************************
 * Filename: client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

/******************************************************************************
 * Initializes the struct_client_info
 *
 * returns struct_client_info*
 * ****************************************************************************/
struct_client_info* cli_init_info(void)
{
    struct_client_info *init;
    int i;
    
    init = CALLOC(struct_client_info);
    if (!init) {
        err_msg("cli_init_info: calloc failure struct_client_info.");
        return NULL;
    }

    init->room = room_init_state(NAME_BUFF);
    if (!init->room) {
        err_msg("cli_init_info: calloc failure - room.");
        return NULL;
    }
    init->name = CALLOC_ARRAY(char, NAME_BUFF);
    if (!init->name) {
        err_msg("cli_init_info: calloc failure - name.");
        return NULL;
    }

    init->f_list = CALLOC(struct_flist);
    if (!init->f_list) {
        err_msg("cli_init_info: calloc failure - f_list.");
        return NULL;
    }

    init->f_list->list = CALLOC_ARRAY(char*, F_MAX);
    if (!init->room) {
        err_msg("cli_init_info: calloc failure - **list.");
        return NULL;
    }
   
    for (i=0; i < F_MAX; ++i) {
        init->f_list->list[i] = CALLOC_ARRAY(char, NAME_BUFF);
        if (!(init->f_list->list[i])) {
            err_msg("cli_init_info: calloc failure - list, index %d",i);
            return NULL;
        }
    }

    return init;
}

/*******************************************************************************
 * Command: /f b
 *
 ******************************************************************************/
int cli_block_enemy(struct_client_info *client, char *name)
{
    return 0;
} /* end block_enemy */

/*******************************************************************************
 *
 *
 *
 *
 ******************************************************************************/
int cli_inv_friend_to_room(struct_client_info *client, char *name)
{
    return 0;
} /* end inv_friend_to_room */

/*******************************************************************************
 *
 *
 *
 *
 ******************************************************************************/
int cli_display_friends(struct_client_info *client)
{
    return 0;
} /* end display_friends */

/*******************************************************************************
 *
 *
 *
 *
 ******************************************************************************/
int cli_remove_friend(struct_client_info *client)
{
    return 0;
} /* end remove_friend */

/*******************************************************************************
 *
 *
 *
 *
 ******************************************************************************/
int cli_add_friend(struct_client_info *client)
{
    return 0;
} /* end add_friend */

/*******************************************************************************
 *
 *
 *
 *
 ******************************************************************************/
int cli_request_room(struct_client_info *client)
{
    return 0;
} /* end request_room */
/****** EOF *****/
