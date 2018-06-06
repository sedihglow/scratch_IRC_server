/*******************************************************************************
 * Filename: server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../include/server.h"

void serv_free_room_list(struct_room_list *room_list)
{
    room_free_all(&(room_list->rooms));
} /* end serv_free_room_list */

/*******************************************************************************
 *
 *  serv_add_client
 *
 *  Add a client to the client list.
 *
 *  TODO: This implementation of the client list is bullshit, doing it this way
 *        for time, should ideally be a proper data structure.
 *
 *  Takes the new_cli pointer and appends it to the old_list, creating the
 *  returned new list.
 *
 *  Old list will be deallocated.
 *
 *  If old_list is null, it will be assumed size 0 and return new_cli.
 *  If old_list is not null but size is still 0, old list will be deallocated
 *  and new_cli will be returned.
 *
 *  size is number of pointers in old_list
 *
 *  new_cli will be set to NULL at end after being placed into the new list.
 *
 *  sets errno to EINVAL if the passed new client is unallocated.
 *  TODO: Untested 
 ******************************************************************************/
struct_cli_info** serv_add_client(struct_cli_info **new_cli, 
                                  struct_cli_info **old_list, size_t old_size)
{
    unsigned int i;
    struct_cli_info **new_list;

    /* make sure there is a new client actually passed */
    if (!new_cli || !*new_cli) {
        errno = EINVAL;
        errnum_msg(EINVAL, "serv_add_client: new_cli == NULL");
        return old_list;
    }

#if 0
    if (!old_list) js
        new_list = CALLOC(struct_cli_info*);
        new_list[0] = *new_cli;
        return new_list;
    } else if (old_size == 0) {
        /* size of zero implies *old_list is NULL */
        FREE_ALL(old_list);
        new_list = CALLOC(struct_cli_info*);
        new_list[0] = *new_cli;
        return new_list;
    }
#endif
    
    new_list = CALLOC_ARRAY(struct_cli_info*, old_size+1);

    for (i=0; i < old_size; ++i)
        new_list[i] = old_list[i];

    new_list[i] = *new_cli;

    free(old_list);

    return new_list;
} /* end serv_add_client() */

/* Free's the client passed */
void serv_free_client(struct_room_list *rooms, struct_cli_info *cli)
{
    int i, ret;

    /* remove from all active rooms, free active room list */
    for (i=0; i < _R_ROOM_MAX; ++i) {
        if (cli->active_rooms[i]) {
            ret = serv_rem_from_room(rooms, cli->active_rooms[i],
                                     cli->name);
            if (ret == FAILURE)
                noerr_msg("serv_free_client: Something failed removing");
            FREE_ALL(cli->active_rooms[i]);
            cli->active_rooms[i] = NULL;
        }
    }

    /* free remaining client information and the former client list. */
    FREE_ALL(cli->name, cli);
} /* end serv_free_client */

/******************************************************************************
 *  serv_remove_client
 *
 *  returns a new list with the removal of the client with name.
 *
 *  sets errno to EINVAL if client name is not in the list
 *  TODO: Untested 
 ******************************************************************************/
struct_cli_info** serv_remove_client(char *name, struct_cli_info **old_list, 
                                     size_t old_size, int sockfd)
{
    unsigned int i, j;
    int ret;
    struct_cli_info **new_list = NULL;
    struct_cli_info *remove = NULL;

    /* check if there is anything to remove */
    if (!old_list)
        return old_list; 

        
    if (name) {
        remove = serv_find_client(name, sockfd, old_list, old_size);
    } else {
        remove = serv_find_fd_client(sockfd, old_list, old_size);
    }

    if (!remove) {
        errno = EINVAL;
        noerr_msg("serv_remove_client: client %s not found in client list.");
        return old_list; 
    }

    if (old_size-1 == 0) {
        FREE_ALL(old_list);
        return NULL;
    }

    new_list = CALLOC_ARRAY(struct_cli_info*, old_size-1);

    /* create new list */
    j = 0; 
    for (i=0; i < old_size; ++i) {
        if (old_list[i] != remove) {
            new_list[j] = old_list[i];
            ++j;
        }
    }

    FREE_ALL(old_list);
    return new_list;
} /* end serv_remove_client() */

/******************************************************************************* 
 * serv_find_client
 *
 *
 * Returns NULL if client is not in the list.
 * Returns pointer to found client struct on success
 ******************************************************************************/
struct_cli_info* serv_find_client(char *find, int fd, struct_cli_info **cli_list, 
                                  size_t size)
{
    unsigned int i;

    for (i=0; i < size; ++i) {
        if (cli_list[i]->name) {
            if (strcmp(cli_list[i]->name, find) == 0)
                return cli_list[i];
        } else if (cli_list[i]->sockfd == fd) {
            return cli_list[i];
        }
    }
    
    return NULL;
} /* end serv_find_client */


/*
 * See description of rmm_add_user in room.c
 */
int serv_add_to_room(struct_room_list *rooms, char *room_name, char *cli_name)
{
    return room_add_user(&rooms->rooms, room_name, cli_name); 
} /* end serv_add_to_room */

/*
 * See description of rmm_add_user in room.c
 */
int serv_rem_from_room(struct_room_list *rooms, char *room_name, char *cli_name)
{
    return room_remove_user(&rooms->rooms, room_name, cli_name);
}

int serv_add_active_room(struct_cli_info *cli, char *room_name)
{
    int i;
    
    for (i=0; i < _R_ROOM_MAX; ++i) {
        if (cli->active_rooms[i] == NULL) {
            cli->active_rooms[i] = CALLOC_ARRAY(char, strlen(room_name)+1);
            if (!cli->active_rooms[i])
                errExit("serv_add_active_room: active_room failed to CALLOC.");
            strcpy(cli->active_rooms[i], room_name);
            return SUCCESS;
        }
    }
    return FAILURE; /* you are at max number of rooms */
} /* end serv_add_active_room */

void serv_remove_active_room(struct_cli_info *cli, char *room_name)
{
    int i;

    for (i=0; i < _R_ROOM_MAX; ++i) {
        if (cli->active_rooms[i]) {
            if (strcmp(cli->active_rooms[i], room_name) == 0) {
                free(cli->active_rooms[i]);
                cli->active_rooms[i] = NULL;
                return;
            }
        }
    }
} /* end serv_remove_active_room */

struct_cli_info* serv_find_fd_client(int fd, struct_cli_info **cli_list, 
                                     size_t size)
{
    unsigned int i;

    for(i=0; i < size; ++i) {
        if (fd == (cli_list[i])->sockfd)
            return cli_list[i];
    }

    return NULL;
} /* end serv_find_fd_client */

/******** EOF *********/
