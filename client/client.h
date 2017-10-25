/*******************************************************************************
 * Filename: client.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../irc_utility.h" /* server or client definitions and functions  */

/* TODO NOTE: basic layout. Nothing in stone, some of these may come from exceptions,
 * signals from other threads, etc. etc. */
int connect_to_server(struct_serv_info *serv_info);
ssize_t send_to_server(int sockfd, char *tx, size_t len, int flags);
ssize_t receive_from_server(int sockfd, char *rx, size_t len, int flags);
#endif
