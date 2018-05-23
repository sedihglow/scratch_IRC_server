/*******************************************************************************
 * Filename: debug.h
 *
 * Written by: James Ross
 ******************************************************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

/*
 * TODO: SCOPE ISSUES.
 *       I want this in its own debug file, but how the client.c handles the
 *       parsing, it will call a function from here at this point.
 *
 *       This means if all parse_args does is return a code, and irc_client
 *       is supposed to call the appropriate functions, then irc_client
 *       will need access to these functions... Which it will!
 *
 *       But its more than 1 header about it, which makes it confusing
 *       potentially, at least not abundantly clear.
 *
 *       Do i declare it in both headers, only in the .c file?
 *       Do i just accept the 2 header layer?
 *       Do i make client.c do all this shit?
 *
 *       This is not quite a test hardness because it comes from commands
 *       in the chat, rather than just a test harness declaring things
 *       like a testbench. Which is why the conflict is happening, it needs to
 *       be defined somewhere but i still want it isolated and would prefer 
 *       irc_client to handle it.
 *
 *       OH!
 *
 *       Maybe make a parse arguments function in this header, which irc_client
 *       calls. Only call it if debug mode is on.
 *
 *       TODO: Make a debug mode since we dont want normal clients to be
 *             able to use it.
 *
 *
 *      maybe nailed it.
 */


/*******************************************************************************
 * TODO: This definition should be a cmd line argument. For now change the 
 *       definition before production.
 ******************************************************************************/
#define DBG_STATE true

/******************************************************************************
 *                      Debug Definitions 
 ******************************************************************************/
#define D_PRINT   "/d yp"      // Turn on debug print statments, sent by pm?
#define D_NOPRINT "/d np"      // Turn off debug print statments
#define D_NETWORK "/d network" // Print any relevent network/comm data
#define D_RLIST   "/d rl"      // Print all existing rooms (even private)
#define D_FLIST   "/d fl"      // /d fl <name> : Print users friend list.
#define D_BLIST   "/d bl"      // /d bl <name> : Print users block list
#define D_FUNCT   "/d funct"   // /d funct <funct> <args...> : exec funct
/* TODO: I think for /d funct, debug needs all the headers for everything just
 *       like a test harness would.
 */

/******************************************************************************
 *                  Debug Command Code Definition
 ******************************************************************************/
#define DRC_PRING    0x0
#define DRC_NOPRINT  0x1
#define DRC_NETWORK  0x2
#define DRC_RLIST    0x4
#define DRC_FLIST    0x8
#define DRC_BLIST    0x10
#define DRC_FUNCT    0x20
#define DRC_ERR      0x40

int find_debug_cmd(char *input);

#endif
/**** EOF ****/
