/*******************************************************************************
 * Filename: client.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "room.h"
#include "irc_comm.h" /* Only for the RC definitions. Dont like it tho. */

/******************************************************************************
 *                      Friend List Definitions
 ******************************************************************************/
#define F_A      "/f a "       // /f a <name>      : Add name to f list
#define F_L      "/f l "       // /f l             : Show friends list
#define F_R      "/f r "       // /f r <name>      : Remove from friends list

/******************************************************************************
 *                      Chat Room Definitions
 ******************************************************************************/
#define JOIN        "/join"    // /join <channel>  : Join dat channel
#define EXIT_IRC    "/exit"    // /exit            : Get the fuck out
#define LEAVE       "/leave"   // /leave <channel> : leave a channel
#define ROOM_L      "/room l"  // /channel l       : List all active
#define ROOM_MSG    "/msg"     // /msg <room> <msg>: send msg to room.
#define ROOM_DISP   "/show"    // /show <room name>: change room display
#define ROOM_USER_L "/room ul" // /room ul         : List members of room
#define LONGEST_CMD_LEN (sizeof(ROOM_USER_L))

/******************************************************************************
 *                      Non Command Definitions
 ******************************************************************************/
#define IO_STR_LEN_MAX H_STR_LEN_MAX // definition found in irc_room.h
#define F_MAX     30    // 30 friends max. Too popular too bad. 

#define CLI_NAME_MAX _NAME_SIZE_MAX 

#define CLI_EXTRA_CHAR_BUFF 10
#define CLI_R_HIST_LEN (_H_STR_LEN_MAX + CLI_NAME_MAX + CLI_EXTRA_CHAR_BUFF)

typedef struct friend_list {
    char *list[F_MAX];
    int fcount;
} struct_flist;

typedef struct client_info {
    struct_room_info *rooms[R_ROOM_MAX];
    int current_r;            /* index of current room */
    int room_count;           /* number of rooms the client is in. */
    char *name;
    struct_flist *f_list;
} struct_client_info;

/* allocation */
struct_client_info* cli_init_info(void);
void cli_free_info(struct_client_info *dest);

/* Sets the client information for a first successful server connection */
int cli_set_new_cli_info(struct_client_info *cli_info, char *name);

/* client handling functions */
int cli_handle_flist(int cmd_type, struct_client_info *client, char *fname);
int cli_add_active_room(struct_client_info *cli, char *room_name);
int cli_remove_active_room(struct_client_info *cli, char *room_name);
int cli_add_to_room_history(struct_client_info *cli, char *room_name,  
                            char *msg, bool disp);
void cli_goto_default_room(struct_client_info *cli_info);
int cli_switch_active_room(struct_client_info *cli_info, char *room_name);
void cli_switch_current_room(struct_client_info *cli_info, char *room_name);
#endif
/****** EOF ******/
