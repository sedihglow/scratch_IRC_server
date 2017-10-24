/*******************************************************************************
 * Filename: irc_server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#define SERV_PORT   31000             /* port listening on server */
#define NET_DOMAIN  AF_INET           /* network domain we are using. IPV4 */
#define IP_PROTOCOL 0                 /* Default for type in socket() */

/* TODO NOTE: basic layout. Nothing in stone, some of these may come from exceptions,
 * signals from other threads, etc. etc. */
int connect_to_server(struct_serv_info *serv_addr);
ssize_t send_to_client(int sockfd, char *tx, size_t len, int flags);
ssize_t recieve_from_server(int sockfd, char *rx, size_t len, int flags);
