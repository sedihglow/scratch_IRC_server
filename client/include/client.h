/*******************************************************************************
 * Filename: client.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "room.h"

/******************************************************************************
 *                      Friend List Definitions
 ******************************************************************************/
#define F_A      "/f a"       // /f a <name>      : Add name to f list
#define F_ADD    "/f add"     // /f add <name>    : Same as above
#define F_L      "/f l"       // /f l             : Show friends list
#define F_LIST   "/f list"    // /f list          : Same as above
#define F_R      "/f r"       // /f r <name>      : Remove from friends list
#define F_REM    "/f remove"  // /f remove <name> : Same as above

#define B_L      "/b l"       //                  : Who you blocked  TODO: limit list size for security
#define B_LIST   "/b list"    //                  : Same as above
#define B_A      "/b a"       // /b a <name>      : Block the fucker
#define B_ADD    "/b add"     // /b add <name>    : same as above
#define B_R      "/b r"       // /b r <name>      : Remove from block list
#define B_REM    "/b remove"  // /b remove <name> : same as above
#define WHO      "/who"       // /who <name>      : Search if a user exists

/******************************************************************************
 *                      Chat Room Definitions
 ******************************************************************************/
#define JOIN     "/join"         // /join <channel>  : Join dat channel
#define EXIT_IRC "/exit"         // /exit            : Get the fuck out
#define VOID     "/leave"        // /leave           : Go to the void
#define LOG_OUT  "/logout"       //                  : Back to login screen.
#define INVITE   "/invite"       // /invite <name>   : Invite into current channel
#define INV      "/inv"          // /inv <name>      : Same as above
// TODO: THERE SHOULD BE A BAN FROM CHANNEL COMMAND. ... should be... ;)
#define L_ROOM   "/room l"       // /channel l       : List all public channels
#define LST_ROOM "/room list"    //                  : Same as above


#define PRIV_MSG "/pm"    // /pm <name> "msg" : Send the private shit
#define PRIV_REP "/r"     // /r "msg"         : Reply to previos PM 

/******************************************************************************
 *                      Debug Definitions TODO: Maybe? move to a debug header? 
 ******************************************************************************/
#define D_PRINT   "/d yp"      // Turn on debug print statments, sent by pm?
#define D_NOPRINT "/d np"      // Turn off debug print statments
#define D_NETWORK "/d network" // Print any relevent network/comm data
#define D_RLIST   "/d rl"      // Print all existing rooms (even private)
#define D_FLIST   "/d fl"      // /d fl <name> : Print users friend list.
#define D_BLIST   "/d bl"      // /d bl <name> : Print users block list
#define D_FUNCT   "/d funct"   // /d funct <funct> <args...> : exec funct

/******************************************************************************
 *                      Non Command Definitions
 ******************************************************************************/
#define IO_STR_LEN_MAX H_STR_LEN_MAX // definition found in irc_room.h
#define IO_MAX    225   // max characters a user can input for a message
#define NAME_BUFF 14    // includes '\0'
#define F_MAX     30    // 30 friends max. Too popular too bad. 

typedef struct friend_list {
    char **list;
} struct_flist;

typedef struct client_info {
    struct_room_state *room;
    char *name;
    struct_flist *f_list;
} struct_client_info;

struct_client_info* cli_init_info(void);
void cli_free_info(struct_client_info *dest);
int cli_block_enemy(struct_client_info *client, char *name);
int cli_inv_friend_to_room(struct_client_info *client, char *name);
int cli_display_friends(struct_client_info *client);
int cli_remove_friend(struct_client_info *client);
int cli_add_friend(struct_client_info *client);
int cli_request_room(struct_client_info *client);

#endif
/****** EOF ******/
