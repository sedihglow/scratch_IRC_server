/*******************************************************************************
 * Filename: client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"

/******************************************************************************* 
 *                          Static Functions
 ******************************************************************************/


/*******************************************************************************
 * TODO: NOTE: Aware of magic numbrs and redundant compares copied into other
 *             functions such as "add ", "a ",etc,etc. Not sure what to do bout
 *             it that would be any more efficient.
 *
 *             Maybe make them each an inline? :/ w/e
 ******************************************************************************/

static int find_fcmd(char *input)
{
    int ret;

    ret = strncmp(input, "add ", 4);
    if (ret == 0)
        return RC_FA;

    ret = strncmp(input, "a ", 2);
    if (ret == 0)
        return RC_FA;

    ret = strncmp(input, "list ", 5);
    if (ret == 0)
        return RC_FL;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_FL;

    ret = strncmp(input, "remove ", 7);
    if (ret == 0)
        return RC_FR;

    ret = strncmp(input, "r ", 2);
    if (ret == 0)
        return RC_FR;

    return RC_ERR;
} /* end find_fcmd */

static int find_bcmd(char *input)
{
    int ret;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_BL;

    ret = strncmp(input, "list ", 5);
    if (ret == 0)
        return RC_BL;

    ret = strncmp(input, "a ", 2);
    if (ret == 0)
        return RC_BA;

    ret = strncmp(input, "add ", 4);
    if (ret == 0)
        return RC_BA;

    ret = strncmp(input, "r ", 2);
    if (ret == 0)
        return RC_BR;

    ret = strncmp(input, "remove ", 6);
    if (ret == 0)
        return RC_BR;

    return RC_ERR;
} /* end find_bcmd */

static int find_inv_cmd(char *input)
{
    int ret;

    ret = strncmp(input, "inv ", 4);
    if (ret == 0)
        return RC_INV;

    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;

    return RC_ERR;
}

static int find_room_cmd(char *input)
{
    int ret;

    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;
      
    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;

    return RC_ERR;
} /* end find_room_cmd */

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
    if (!init->f_list->list) {
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

    FREE_ALL(dest->room, dest->name, dest->f_list->list, dest->f_list, dest);
}

/*******************************************************************************
 * Parse's the input of the client. Finds what type of input it was, msg or 
 * command then which command if applicable.
 *
 * Returns identifier showing which type of input it was.
 *
 * Some functions pass an incrented address so we dont compare the same shit
 * twice. Only one that doesnt in /inv vs /invite since there is no space
 * seperator.
 *
 * TODO: This might be threaded and called after input, on the thread that runs 
 *       the input waiting. Then pass this return value to main thread.
 ******************************************************************************/
int parse_args(char *input)
{
    int ret = 0;

    ret = strncmp(input, "/f ", 3);
    if (ret == 0)
        return find_fcmd(input+3);

    ret = strncmp(input, "/b ", 3);
    if (ret == 0)
        return find_bcmd(input+3);

    ret = strncmp(input, WHO, sizeof(WHO));
    if (ret == 0)
        return RC_WHO;

    ret = strncmp(input, JOIN, sizeof(JOIN));
    if (ret == 0)
        return RC_JOIN;

    ret = strncmp(input, LOG_OUT, sizeof(LOG_OUT));
    if (ret == 0)
        return RC_LOGOUT;

    ret = strncmp(input, INV, sizeof(INV));
    if (ret == 0)
        return find_inv_cmd(input);

    ret = strncmp(input, ROOM_L, sizeof(ROOM_L));
    if (ret == 0)
        return find_room_cmd(input+5);

    ret = strncmp(input, PRIV_MSG, sizeof(PRIV_MSG));
    if (ret == 0)
        return RC_PM;

    /* This needs to be AFTER comparing ROOM_L since both start with /r */
    ret = strncmp(input, PRIV_REP, sizeof(PRIV_REP));
    if (ret == 0)
        return RC_PR;

    ret = strncmp(input, VOID, sizeof(PRIV_REP));
    if (ret == 0)
        return RC_VOID;

    ret = strncmp(input, EXIT_IRC, sizeof(EXIT_IRC));
    if (ret == 0)
        return RC_EXIT;


    return RC_MSG;
} /* end parse_args */

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
