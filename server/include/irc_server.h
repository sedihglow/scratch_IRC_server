/*******************************************************************************
 * Filename: irc_server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "comm.h"
#include "server.h"

typedef struct irc_info {
    int *full_fd_list; /* server + client */
    int num_fds; /* current fd list size */
    struct_serv_info *serv_info;
    struct_cli_info **cli_list; /* memory handled in add/remove functions */
    int num_clients;
} struct_irc_info;

void irc_server(void);

struct_irc_info* irc_init_info(void);
void irc_free_info(struct_irc_info *info);

int irc_manage_select(void);
void irc_fill_fd_set_read(struct_irc_info *irc_info, fd_set *readfds);
int* irc_add_fd_list(struct_irc_info *info, int newfd);
int* irc_remove_fd_list(struct_irc_info *info, int fd);
