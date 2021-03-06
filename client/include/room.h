/******************************************************************************
 * Filename: room.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_room.h" // struct_room_info found here.

/* NOTE: Definitions starting with _ are found in shared folder irc_room.h */
#define USR_MAX        _R_USR_MAX
#define H_STR_MAX      _H_STR_MAX
#define H_STR_LEN_MAX  _H_STR_LEN_MAX  
#define R_NAME_LEN_MAX _R_NAME_LEN_MAX 
#define R_PW_LEN_MAX   _R_PW_LEN_MAX   
#define R_DFLT_ROOM    _R_DFLT_ROOM    
#define R_ROOM_MAX     _R_ROOM_MAX

struct_room_info*  room_init_info(void);
void room_free_info(struct_room_info *dest);

int room_add_to_history(struct_room_info *room, char *msg);
/***** EOF *****/
