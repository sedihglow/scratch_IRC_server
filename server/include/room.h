/*******************************************************************************
 * Filename: room.h
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_room.h" // TODO: Not sure how i should jugle this header w/ tree
#include "red_black.h"

/*
 * TODO:
 *  Make wrapper around red_black functions for server.h to use. Make the names
 *  all friends for the program we are doing.
 *   - Thinking about making them inline if all they do is call one function
 *  
 *  This will achieve a base for these operations, not neccisarily all 1 funct
 *  - creating a room
 *  - finding if a room exists
 *  - managing the room info struct and all of its contents.
 *  - adding to history, rooms history ring buffers will be handles here 
 *  - adding a user to a room
 *  - removing a user from a room
 *  - deleting a room if no one is in it to prevent bad things.
 *  - Will need to be able to identify a unique person in a room
 *      - Maybe make a lookup table of clients?
 *          struct room_user {
 *              void *client_info
 *          }
 *
 *          User this in the room_info struct instead of char *room_users**
 *          Will fill in the information with passed data from server.h,
 *          keeping client info seperate.
 * - will need to be able to remove someone from a room
 * - Check if requested room access is private or public. If private, deny
 *   if no password present or password is incorrect.
 *
 *
 *   I think these functions should be kept fairly simple and only focus on
 *   altering the nodes information, and returing the proper things
 *   so server.h can handle what to do with the clients.
 *
 *   May have the node in the red_black tree be void so i can just store
 *   the clients socket_info and return that if need be. Might be completely
 *   unneccisary depending on how i implement server.h
 */

/* TODO: All untested */
struct_room_info* room_find(rbTree *rooms, char *name);
int room_create(rbTree *rooms, struct_room_info *new_room);
int room_add_history(rbTree *room_list, char *room_name, char *msg);
int room_add_user(rbTree *room_list, char *room_name, char *cli_name);
int room_remove_user(rbTree *room_list, char *room_name, char *cli_name);
int room_free(rbTree *room_list, char *room_name);

int room_check_for_client(char *cli_name, struct_room_info *room_info);
//int room_check_pw();
