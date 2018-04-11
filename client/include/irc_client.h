/*******************************************************************************
 * Filename: irc_client.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "client.h"
#include "comm.h" // holds struct_serv_info

typedef struct irc_info {
    struct_client_info *client;
    struct_serv_info   *serv_info;
    struct_io_ring     *rx;
    struct_io_ring     *tx;
} struct_irc_info;

/* Function executed by main to run the irc client */
void free_irc_info(struct_irc_info);
void irc_client(void);
