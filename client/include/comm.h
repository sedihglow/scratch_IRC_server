/******************************************************************************
 * Filename: comm.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _COMM_H_
#define _COMM_H_

#include "irc_comm.h" // NOTE: holds struct_server_info and struct_io_ring

struct_serv_info* com_init_serv_info(void);
struct_io_ring* com_init_io_ring(void);
void com_free_serv_info(struct_serv_info *dest);
void com_free_io_ring(struct_io_ring *dest);

int init_client_comm(struct_serv_info *serv_info);
ssize_t send_to_server(int sockfd, uint8_t *tx, size_t len, int flags);
ssize_t receive_from_server(int sockfd, uint8_t *rx, size_t len, int flags);
int request_who(char *name, bool *online); 

/* returns a build message to be sent. */
int com_send_logon_message(char *name, struct_serv_info *serv_info);
int com_get_logon_result(int fd);
#endif
