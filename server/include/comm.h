/*******************************************************************************
 * Filename: comm.c
 *
 * Written by: James Ross
 *
 * Handles all server connections and communication functions
 ******************************************************************************/

#include "irc_comm.h"

/* _COM definitions found in irc_comm.h */
#define SERV_ADDR   _COM_SERV_ADDR
#define SERV_LEN    _COM_SERV_LEN    
#define SERV_PORT   _COM_SERV_PORT   
#define NET_DOMAIN  _COM_NET_DOMAIN  
#define SOCK_TYPE   _COM_SOCK_TYPE   
#define IP_PROTOCOL _COM_IP_PROTOCOL 
#define IO_BUFF     _COM_IO_BUFF 

#define LQ_SIZE 15  /* the backlog queue suggested in listen() */

int open_connection(struct_serv_info *serv_info);
int init_server_comm(struct_serv_info *serv_info);
ssize_t send_to_client(int sockfd, char *tx, size_t len, int flags);
ssize_t receive_from_client(int sockfd, char *rx, size_t len, int flags);

struct_cli_message* com_parse_cli_message(char *rx);
