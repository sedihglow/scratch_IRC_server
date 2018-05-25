/*******************************************************************************
 * Filename: irc_client.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _H_IRC_CLIENT_H
#define _H_IRC_CLIENT_H

#include "client.h"
#include "comm.h" // holds struct_serv_info
#include "debug.h"


typedef struct irc_info {
    struct_client_info *client;
    struct_serv_info   *serv_info;
    struct_io_ring     *tx;
    struct_io_ring     *rx;
} struct_irc_info;

/* Function executed by main to run the irc client */
int parse_args(char *input);
void* irc_handle_server_requests(void *args);
char* irc_get_user_input(void);

struct_irc_info* init_irc_info(void);
void irc_free_info(struct_irc_info *dest);
void irc_client(void);

/* Display Functions */
void display_welcome(void);

void irc_logon_client(struct_irc_info *irc_info);
#endif
/****** EOF *****/
