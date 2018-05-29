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
    
    for (i=0; i < R_ROOM_MAX; ++i)
        init->rooms[0] = NULL;

    init->name = NULL;

    /* flist struct */
    init->f_list = CALLOC(struct_flist);
    if (!init->f_list) {
        err_msg("cli_init_info: calloc failure - f_list.");
        return NULL;
    }
   
    return init;
} /* end cli_init_info() */

void cli_free_info(struct_client_info *dest)
{
    int i;

    for (i=0; i < R_ROOM_MAX; ++i) { 
        if (dest->rooms[i])
            room_free_info(dest->rooms[i]);
    }

    /* free flist->list indecies */
    for (i=0; i < F_MAX; ++i) {
        if (dest->f_list->list[i])
            free(dest->f_list->list[i]);
        else
            break;
    }

    FREE_ALL(dest->name, dest->f_list->list, dest->f_list, dest);
} /* end cli_free_info */

int cli_set_new_cli_info(struct_client_info *cli_info, char *name)
{
    int len;

    /* client info shouldnt be passed initial init function. name is NULL */
    if (!name || cli_info->name != NULL)
        return FAILURE;

    len = strlen(name) + 1;
    cli_info->name = CALLOC_ARRAY(char, len);
    if (!cli_info->name)
        return FAILURE;

    strncpy(cli_info->name, name, len);

    cli_info->rooms[0] = room_init_info(); /* allocates initial room */

    cli_info->rooms[0]->room_name = CALLOC_ARRAY(char, sizeof(R_DFLT_ROOM));
    if (!cli_info->rooms[0]->room_name)
        return FAILURE;

    cli_info->rooms[0]->room_name = strncpy(cli_info->rooms[0]->room_name, 
                                            R_DFLT_ROOM, sizeof(R_DFLT_ROOM));
    cli_info->room_count = 1;
    cli_info->current_r = 0;

    /* Should be a function for this shit
    cli_info->rooms = CALLOC(struct_room_info);
    if (!cli_info->rooms)
        return FAILURE;
    */

    return SUCCESS;
} /* end cli_set_new_cli_info */


/*******************************************************************************
 *                      Handle Friends List
 ******************************************************************************/

static void cli_display_friends(struct_flist *flist);
static int cli_add_friend(struct_flist *flist, char *fname);
static int cli_remove_friend(struct_flist *flist, char *fname);

/*******************************************************************************
 * Command: /f l
 ******************************************************************************/
static void cli_display_friends(struct_flist *flist)
{
    int i;
    printf("---- Friends ----\n Total on list: %d\n\n", flist->fcount);

    for (i=0; i < F_MAX; ++i) {
        if (flist->list[i])
            printf("%s\n",flist->list[i]);
    }
} /* end display_friends */


/*******************************************************************************
 * Command: /f a
 ******************************************************************************/
static int cli_add_friend(struct_flist *flist, char *fname)
{
    int i;

    for (i=0; i < F_MAX; ++i) {
        if (!flist->list[i]) {               /* find open spot */
            my_strdup(flist->list[i], fname, strlen(fname)+1); /* duplicate */
            if (flist->list[i]) {            /* error check */
                ++flist->fcount;
                return SUCCESS;
            } else {
                err_msg("cli_add_friend: strdup failed, ret NULL.");
                return FAILURE;
            }
        }
    }
    
    printf("Friend list is full. Remove some poor person and try again.\n\n");
    return FAILURE;
} /* end add_friend */


/*******************************************************************************
 * Command: /f r
 ******************************************************************************/
static int cli_remove_friend(struct_flist *flist, char *fname)
{
    int i;
    int ret;

    for (i=0; i < F_MAX; ++i) {
        if (flist->list[i]) {
            ret = strcmp(flist->list[i], fname); 
            if (ret == 0) {
                free(flist->list[i]);
                return SUCCESS;
            }
        }
    }

    printf("%s was not found. Nothing to remove.\n\n", fname);
    return FAILURE;
} /* end remove_friend */


/******************************************************************************* 
 * cli_handle_flist
 *
 * take a request from the user and execute the proper function.
 *
 * Acceptable commands, RC_FL, RC_FA, RC_FR
 *
 * fname gets ignored in RC_FL
 *
 * returns SUCCESS, request accomplished.
 *         FAILURE, invalid command type for flist
 ******************************************************************************/
int cli_handle_flist(int cmd_type, struct_client_info *client, char *fname)
{
    int ret; 

    switch (cmd_type) {
    case RC_FL:
        cli_display_friends(client->f_list);
        ret = SUCCESS;
    break;
    case RC_FA:
        ret = cli_add_friend(client->f_list, fname);
    break;
    case RC_FR:
        ret = cli_remove_friend(client->f_list, fname);
    break;
    default: 
        errnum_msg(EINVAL, "cli_handle_flist: invalid cmd type");
        return FAILURE;
    }
    return ret; 
}/* end cli_handle_flist */


#if 0
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
 * Command: /invite
 ******************************************************************************/
int cli_inv_friend_to_room(struct_client_info *client, char *name)
{
    return 0;
} /* end inv_friend_to_room */
#endif
/****** EOF *****/
