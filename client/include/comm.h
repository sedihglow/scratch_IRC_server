/******************************************************************************
 * Filename: comm.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _COMM_H_
#define _COMM_H_

#include "irc_comm.h" // NOTE: holds struct_server_info and struct_io_ring

#define IO_BUFF  _COM_IO_BUFF

struct_serv_info* com_init_serv_info(void);
void com_free_serv_info(struct_serv_info *dest);

int init_client_comm(struct_serv_info *serv_info);

/* core send/recv functions */
ssize_t send_to_server(int sockfd, uint8_t *tx, size_t len, int flags);
ssize_t receive_from_server(int sockfd, uint8_t *rx, size_t len, int flags);

/* sends properly built message to the server. */
int com_send_logon_message(char *name, struct_serv_info *serv_info);
int com_send_chat_message(char *cli_name, char *room_name, char *msg, 
                          struct_serv_info *serv_info);
int com_send_join_message(char *cli_name, char *room_name,
                          struct_serv_info *serv_info);
int com_send_leave_message(char *cli_name, char *room_name, 
                           struct_serv_info *serv_info);
int com_send_logout_message(char *cli_name, struct_serv_info *serv_info);
int com_send_exit_message(char *cli_name, struct_serv_info *serv_info);
int com_send_room_users_message(char *cli_name, char *room_name, 
                        struct_serv_info *serv_info);

/* parses message from the server to the client */
struct_serv_message* com_parse_server_msg(uint8_t *input);

/* recieving logon result from server */
int com_get_logon_result(int fd);
#endif
