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
    if (dest->f_list) {
    /* free flist->list indecies */
        for (i=0; i < F_MAX; ++i) {
            if (dest->f_list->list[i])
                free(dest->f_list->list[i]);
        }
    }

    FREE_ALL(dest->name, dest->f_list, dest);
} /* end cli_free_info */


int cli_add_to_room_history(struct_client_info *cli, char *room_name,  char *msg,
                            bool disp)
{
    int i;

    /* store the message in the history ring buffer. */
    for (i=0; i < R_ROOM_MAX; ++i) {
        if (strcmp(room_name, cli->rooms[i]->room_name) == 0) {
            /* found the room to add message. */
            if (room_add_to_history(cli->rooms[i], msg) == FAILURE)
                return FAILURE;

            if (disp == true)
                printf("%s\n", msg);
            return SUCCESS; 
        }
    }

    return FAILURE;
} /* end cli_add_to_history */


/* this is what /void will basically do. */
void cli_goto_default_room(struct_client_info *cli_info)
{
    int i; 

    /* make sure all rooms are left */
    for (i=0; i < R_ROOM_MAX; ++i) {
        if (cli_info->rooms[i])
            room_free_info(cli_info->rooms[i]);
    }

    cli_info->rooms[0] = room_init_info(); /* allocates initial room */

    cli_info->rooms[0]->room_name = CALLOC_ARRAY(char, sizeof(R_DFLT_ROOM));
    if (!cli_info->rooms[0]->room_name)
        errExit("cli_goto_default_room: room_name failed to calloc.");

    cli_info->rooms[0]->room_name = strncpy(cli_info->rooms[0]->room_name, 
                                            R_DFLT_ROOM, sizeof(R_DFLT_ROOM));
    cli_info->room_count = 1;
    cli_info->current_r = 0;
} /* end cli_goto_default_room */

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
    
    cli_goto_default_room(cli_info);
    /* Should be a function for this shit
    cli_info->rooms = CALLOC(struct_room_info);
    if (!cli_info->rooms)
        return FAILURE;
    */

    return SUCCESS;
} /* end cli_set_new_cli_info */

/*
 * Anytime a new room gets added it becomes your forward active room
 */
int cli_add_active_room(struct_client_info *cli, char *room_name)
{
    int i;
    
    for (i=0; i < _R_ROOM_MAX; ++i) {
        if (cli->rooms[i] == NULL) {

            cli->rooms[i] = room_init_info();

            cli->rooms[i]->room_name = CALLOC_ARRAY(char, strlen(room_name)+1);
            if (!cli->rooms[i]->room_name)
                errExit("serv_add_active_room: room name failed to CALLOC.");

            strcpy(cli->rooms[i]->room_name, room_name);
            cli->current_r = i;
            return SUCCESS;
        }
    }
    return FAILURE; /* you are at max number of rooms */
} /* end serv_add_active_room */

/*
 * If removing an active room currently being displayed, it takes you to the
 * first room it finds in your list
 *
 */
int cli_remove_active_room(struct_client_info *cli, char *room_name)
{
    int i, ret;
    bool flag = false;

    for (i=0; i < _R_ROOM_MAX; ++i) {
        if (cli->rooms[i]) {
            if (strcmp(cli->rooms[i]->room_name, room_name) == 0) {
                room_free_info(cli->rooms[i]);
                cli->rooms[i] = NULL;
                flag = true;
                break;
            } 
        }
    }

    /* find a valid index to return. Race condition if doing it in first loop */
    for (i = 0; i < _R_ROOM_MAX; ++i) {
        if (cli->rooms[i]) {
            cli->current_r = i;
            return SUCCESS;
        }
    }

    if (flag) {
        cli->current_r = 0;
        return SUCCESS;
    }
    return  FAILURE;
} /* end serv_remove_active_room */

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
    printf("---- Friends ----\n Total on list: %d\n", flist->fcount);

    for (i=0; i < F_MAX; ++i) {
        if (flist->list[i])
            printf("%s\n",flist->list[i]);
    }
    printf("\n");
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
                flist->list[i] = NULL;
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


int cli_switch_active_room(struct_client_info *cli_info, char *room_name)
{
    int i, ret;

    for (i=0; i < R_ROOM_MAX; ++i) {
        if (cli_info->rooms[i]) {
            ret = strncmp(cli_info->rooms[i]->room_name, room_name, 
                         strlen(room_name) + 1);
            if (ret == 0) {
                cli_info->current_r = i;
                return SUCCESS;
            }
        }
    }
    return FAILURE;
} /* end cli_switch_active_room */


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
