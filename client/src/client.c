/*******************************************************************************
 * Filename: client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

/******************************************************************************* 
 *                          Static prototypes 
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

void cli_free_info(struct_client_info *dest)
{
    int i;

    room_free_state(dest->room);
    
    /* free flist->list indecies */
    for (i=0; i < F_MAX; ++i)
        free(dest->f_list->list[i]);

    FREE_ALL(dest->room, dest->name, dest->f_list->list, dest->f_list);
    dest->room   = NULL;
    dest->name   = NULL;
    dest->f_list = NULL;
}

/*******************************************************************************
 * Parse's the input of the client. Finds what type of input it was, msg or 
 * command then which command if applicable.
 *
 * Returns identifier showing which type of input it was.
 *
 * TODO: This might be threaded and called after input, on the thread that runs 
 *       the input waiting. Then pass this return value to main thread.
 ******************************************************************************/
int parse_args(char *input)
{
    int ret = 0;

    ret = strncmp(input, "/f", 2);
    if (ret == 0) {
    }

    ret = strncmp(input, "/b", 2);
    if (ret == 0) {
    }

    ret = strncmp(input, WHO, sizeof(WHO));
    if (ret == 0) {
    }

    ret = strncmp(input, JOIN, sizeof(JOIN));
    if (ret == 0) {
    }

    ret = strncmp(input, LOG_OUT, sizeof(LOG_OUT));
    if (ret == 0) {
    }

    ret = strncmp(input, INVITE, sizeof(LOG_OUT));
    if (ret == 0) {
    }

    ret = strncmp(input, INV, sizeof(INV));
    if (ret == 0) {
    }

    ret = strncmp(input, L_ROOM, sizeof(L_ROOM));
    if (ret == 0) {
    }

    ret = strncmp(input, LST_ROOM, sizeof(LST_ROOM));
    if (ret == 0) {
    }

    ret = strncmp(input, PRIV_MSG, sizeof(PRIV_MSG));
    if (ret == 0) {
    }

    ret = strncmp(input, PRIV_REP, sizeof(PRIV_REP));
    if (ret == 0) {
    }

    ret = strncmp(input, EXIT_IRC, sizeof(EXIT_IRC));
    if (ret == 0) {
    }

    return 0;
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
