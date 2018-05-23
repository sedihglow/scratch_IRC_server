/*******************************************************************************
 * Filename: server.c
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../include/server.h"

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

    if (!new_cli || !*new_cli) {
        printf("new cli: %x, *new_cli, %x");
        errno = EINVAL;
        errnum_msg(EINVAL, "serv_add_client: new_cli == NULL");
        return NULL;
    }

    if (!old_list) {
        new_list = CALLOC(struct_cli_info*);
        new_list[0] = *new_cli;
        *new_cli = NULL;
        return new_list;
    }
    else if (old_size == 0) {
        FREE_ALL(old_list[0], old_list);
        new_list = CALLOC(struct_cli_info*);
        new_list[0] = *new_cli;
        *new_cli = NULL;
        return new_list;
    }
    
    new_list = CALLOC_ARRAY(struct_cli_info*, old_size+1);

    for (i=0; i < old_size; ++i)
        new_list[i] = old_list[i];

    new_list[i] = *new_cli;

    *new_cli = NULL;

    free(old_list);

    return new_list;
} /* end serv_add_client() */

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
    struct_cli_info **new_list;
    struct_cli_info *remove = NULL;

    /* check if there is anything to remove */
    if (!old_list) {
        return NULL; 
    }
    else if (old_size < 2) {
        FREE_ALL(old_list[0], old_list);
        *old_list = NULL;
        return NULL;
    }
    
    new_list = CALLOC_ARRAY(struct_cli_info*, old_size-1);
    
    if (name)
        remove = serv_find_client(name, old_list, old_size);
    else
        remove = serv_find_fd_client(sockfd, old_list, old_size);

    if (!remove) {
        errno = EINVAL;
        noerr_msg("serv_remove_client: client %s not found in client list.");
        return NULL; 
    }

    /* create new list */
    j = 0;
    for (i=0; i < old_size; ++i) {
        if (old_list[i] != remove) {
            new_list[j] = old_list[i];
            ++j;
        }
    }

    FREE_ALL(remove, old_list);

    return new_list;
} /* end serv_remove_client() */

/******************************************************************************* 
 * serv_find_client
 *
 *
 * Returns NULL if client is not in the list.
 * Returns pointer to found client struct on success
 *  TODO: Untested 
 ******************************************************************************/
struct_cli_info* serv_find_client(char *find, struct_cli_info **cli_list, 
                                  size_t size)
{
    unsigned int i;

    for (i=0; i < size; ++i) {
        if (strcmp(cli_list[i]->name, find) == 0)
            return cli_list[i];
    }
    
    return NULL;
} /* end serv_find_client */


/*
 * See description of rmm_add_user in room.c
 */
int serv_add_to_room(struct_room_list *rooms, char *room_name)
{
    return room_add_user(&rooms->rooms, room_name); 
} /* end serv_add_to_room */

/*
 * See description of rmm_add_user in room.c
 */
int serv_rem_from_room(struct_room_list *rooms, char *room_name, char *cli_name)
{
    return room_remove_user(&rooms->rooms, room_name, cli_name);
}



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
