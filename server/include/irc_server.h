/*******************************************************************************
 * Filename: irc_server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "comm.h"
#include "server.h"

#define DELETE_CLI "c@tz"

typedef struct irc_info {
    int *full_fd_list;  /* server + client */
    int num_fds;        /* current fd list size */
    struct_serv_info *serv_info;
    struct_cli_info **cli_list; /* memory handled in add/remove functions */
    struct_room_list *rooms;
    int num_clients;
} struct_irc_info;

void irc_server(void);

struct_irc_info* irc_init_info(void);
void irc_free_info(struct_irc_info *info);

int irc_shutdown_client(struct_irc_info *irc_info, struct_cli_info *cli_info);

void irc_fill_fd_set_read(struct_irc_info *irc_info, fd_set *readfds);
int* irc_add_fd_list(struct_irc_info *info, int newfd);
int* irc_remove_fd_list(struct_irc_info *info, int fd);
void irc_take_new_connection(int *nfds, struct_irc_info *irc_info);


int irc_handle_cli(struct_irc_info *irc_info, struct_cli_info *cli_info);

/* TODO: Incomplete, not setting clients TX buffer yet. */
int irc_accept_new_cli(struct_irc_info *irc_info, struct_cli_message *cli_msg,
                       struct_cli_info *cli);
int irc_cli_msg_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);

int irc_cli_join_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);
int irc_cli_leave_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);


int irc_send_msg_to_cli_tx(struct_cli_info *cli_info, char *msg);


int irc_cli_exit_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);
/***** EOF *****/

