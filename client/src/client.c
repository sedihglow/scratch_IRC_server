/*******************************************************************************
 * Filename: client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

/******************************************************************************* 
 *                          Header Functions
 ******************************************************************************/

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

    init->rooms = NULL;
    init->name = NULL;

    init->f_list = CALLOC(struct_flist);
    if (!init->f_list) {
        err_msg("cli_init_info: calloc failure - f_list.");
        return NULL;
    }

    init->f_list->list = CALLOC_ARRAY(char*, F_MAX);
    if (!init->f_list->list) {
        err_msg("cli_init_info: calloc failure - **list.");
        return NULL;
    }
   
    return init;
} /* end cli_init_info() */

void cli_free_info(struct_client_info *dest)
{
    int i;

    room_free_info(dest->rooms);

    /* free flist->list indecies */
    for (i=0; i < F_MAX; ++i) {
        if (dest->f_list->list[i])
            free(dest->f_list->list[i]);
        else
            break;
    }

    FREE_ALL(dest->rooms, dest->name, dest->f_list->list, dest->f_list, dest);
}

/*******************************************************************************
 * Command: /f a
 ******************************************************************************/
int cli_add_friend(struct_client_info *client)
{
    return 0;
} /* end add_friend */

/*******************************************************************************
 * Command: /f l
 ******************************************************************************/
int cli_display_friends(struct_client_info *client)
{
    return 0;
} /* end display_friends */

/*******************************************************************************
 * Command: /f r
 ******************************************************************************/
int cli_remove_friend(struct_client_info *client)
{
    return 0;
} /* end remove_friend */

/*******************************************************************************
 * Command: /b l
 ******************************************************************************/
int cli_display_block(struct_client_info *client, char *name)
{
    return 0;
} /* end block_enemy */

/*******************************************************************************
 * Command: /b a
 ******************************************************************************/
int cli_block_enemy(struct_client_info *client, char *name)
{
    return 0;
} /* end block_enemy */

/*******************************************************************************
 * Command: /b r
 ******************************************************************************/
int cli_remove_block(struct_client_info *client, char *name)
{
    return 0;
} /* end block_enemy */

/*******************************************************************************
 * Command: /join 
 ******************************************************************************/
int cli_request_room(struct_client_info *client)
{
    return 0;
} /* end request_room */

/*******************************************************************************
 * Command: /invite
 ******************************************************************************/
int cli_inv_friend_to_room(struct_client_info *client, char *name)
{
    return 0;
} /* end inv_friend_to_room */

/****** EOF *****/
