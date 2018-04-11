/******************************************************************************
 * Filename: comm.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _COMM_H_
#define _COMM_H_

#include "irc_comm.h" // holds struct_server_info
#include "utility_sys.h"

int init_client_comm(struct_serv_info *serv_info);
ssize_t send_to_server(int sockfd, char *tx, size_t len, int flags);
ssize_t receive_from_server(int sockfd, char *rx, size_t len, int flags);
#endif
