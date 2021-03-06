/*******************************************************************************
 * Filename: irc_server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "comm.h"
#include "server.h"

#define EXIT_CMD "/exit"
#define EXIT_NUM 0x0
#define DIRECT_IO_SIZE 10

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

/* socket connections and info data */
int irc_accept_new_cli(struct_irc_info *irc_info, struct_cli_message *cli_msg,
                       struct_cli_info *cli);
void irc_take_new_connection(int *nfds, struct_irc_info *irc_info);
int irc_shutdown_client(struct_irc_info *irc_info, struct_cli_info *cli_info);

/* set select read file descriptors from fd list */
void irc_fill_fd_set_read(struct_irc_info *irc_info, fd_set *readfds);

/* file descriptor list manipulation */
int* irc_add_fd_list(struct_irc_info *info, int newfd);
int* irc_remove_fd_list(struct_irc_info *info, int fd);


/* receive from client and handle the request */
int irc_handle_cli(struct_irc_info *irc_info, struct_cli_info *cli_info);

/* functions executed based on command from the client */
int irc_cli_msg_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);
int irc_cli_join_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);
int irc_cli_leave_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);
int irc_cli_list_room_users(struct_irc_info *irc_info, 
                            struct_cli_message *cli_msg);
int irc_send_msg_to_cli_tx(struct_cli_info *cli_info, char *msg);
int irc_cli_exit_cmd(struct_irc_info *irc_info, struct_cli_message *cli_msg);

/* direct input for server admin commands */
int irc_check_direct_input(void);

/***** EOF *****/

