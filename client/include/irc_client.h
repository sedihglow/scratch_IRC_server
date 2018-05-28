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

/* initial allocation and free. Free will work with any allocation state */
struct_irc_info* init_irc_info(void);
void irc_free_info(struct_irc_info *dest);

/* Function executed by main to run the irc client */
void irc_client(void);
void irc_logon_client(struct_irc_info *irc_info);

/* client input */
char* irc_get_user_input(void);
int irc_handler_user_input(struct_irc_info *irc_info, char *input);


/* Display Functions */
void display_welcome(void);
void display_clear(void);
void display_room_welcome(char *room_name, int num_users);

/* thread to recieve and update data */
void* irc_handle_server_requests(void *args);

#endif
/****** EOF *****/
