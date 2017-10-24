/*******************************************************************************
 * Filename: server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "../irc_utility.h"

#define LQ_SIZE 15  /* the backlog queue suggested in listen() */

/* TODO NOTE: basic layout. Nothing in stone, some of these may come from exceptions,
 * signals from other threads, etc. etc. */
int open_connection(struct_serv_info *serv_info);
ssize_t send_to_client(int sockfd, char *tx, size_t len, int flags);
ssize_t recieve_from_client(int sockfd, char *rx, size_t len, int flags);
