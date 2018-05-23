/******************************************************************************
 *  Filename: red_black.h
 *
 *  Header file for a red_black tree data structure,
 *
 *  Written by: James Ross
 ******************************************************************************/


                /* headers */

#ifndef __RBTREE_H__
#define __RBTREE_H__

#include "irc_room.h"


#define CHILD 2  /* ammout of children per node */
#define BLACK 0  /* black color flag */
#define RED   1  /* red color flag */
#define LEFT  0  /* left child */
#define RIGHT 1  /* right child */


/* retreive the color of the given rbNode */
#define find_rbcolor(node) (((node) == NULL) ? BLACK : (node) -> cflag)

/* finds a nodes grandparent, pointer, or NULL if non existant */
#define find_parent(node)  (((node) != NULL) ? (node) -> parent : NULL)

/* finds the sibling of a node */
#define find_sibling(node) (((node) == (node) -> parent -> child[LEFT])        \
                              ? (node) -> parent -> child[RIGHT]               \
                              : (node) -> parent -> child[LEFT])

/* finds a nodes uncle, pointer, or NULL if non existant */
#define find_uncle(node, grand) (((grand)) /* if grand exists get uncle */     \
                                ? (                                            \
                                    ((node) -> parent == (grand) -> child[LEFT])\
                                    ? (grand) -> child[RIGHT]                  \
                                    : (grand) -> child[LEFT])                  \
                                : NULL) /* else NULL */

/* node the tree contains */

/****************************************************************************** 
 * TODO: Each node needs to contain the room data. Replace the data pointer
 *       with either explicitly room data, or with a void pointer.
 *
 *       I remember big issues using hte void pointer tho... :/
 *
 *       So maybe say fuck it for this assignment and just do it not as legit
 *
 *       with the red black tree not as abstract.
 *          
 *      Maybe rephrase this to-do and keep it there for future reference that
 *      i will probably never get to.
 *
 *     
 *
 *
 *      TODO: 
 *              - Anything returning data
 *              - Anything accepting data to add.
 *              - May need to have it just add an updated room instead of
 *                passing values that are just to go into the room like
 *                history and bullshit.
 ******************************************************************************/


/* TODO: Why did i think it was okay to use return 1 for success? D: */

typedef struct RBnode
{
    struct_room_info *room;    /* pointer to room information in node */
    uint32_t cflag;             /* black = 0, red = 1 */

    struct RBnode *parent;      /* parent of current node */
    struct RBnode *child[CHILD];/* child 1/2 (index 0/1) */
} rbNode;

/* holds a root pointer to an entire RB tree */
typedef struct RBtree
{
    struct RBnode *root;
} rbTree; 

                /* initializations */
/* initialize a rb tree, returns 1 if success 0 if error */
int rbTree_init(rbTree *tree);

                /* insertion */
/* give a piece of data to a tree, return 1 if success, 0 if error */
uint32_t give_data(rbTree *tree, struct_room_info *toAdd);


               /* removal */
/* removes the first instance of a piece of data in the tree, return 1 if
   success, 0 if toRemove was not in the tree */
uint32_t remove_first(rbTree *tree, char *toRemove); /* wrapper for first_remove */

/* removes each node that contains toRemove's data. Returns number of nodes
   removed */
uint32_t remove_each(rbTree *tree, char *toRemove);

/* removes everything from the tree. returns 1 if success, 0 if failure */
void remove_all(rbTree *tree); /* wrapper for remove_rbTree() */
             

              /* utility functions */
/* count every instance of a piece of data, return count */
uint32_t data_count(rbTree *tree, const char *room_name); /* count_data wrapper */

/* finds the first piece of data and returns the node's address */

/*******************************************************************************
 * TODO: This is the retrieve function, It should not return a node anymore,
 *       it should just return the room information struct
 ******************************************************************************/
struct_room_info* find_match(rbTree *tree, char *toFind);

/* return the total ammount of nodes in the rbTree */
uint32_t total_data_count(rbTree *tree);

            
             /* display functions */
/* display every nodes data, and its color/flag */
uint32_t display_all(rbTree *tree); /* wrapper for display_rbTree */
#endif
/*** EOF ***/
