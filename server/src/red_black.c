/* 
    author: James Ross

    not for distribution, permittion to use needs to be approved beforehand.
*/
#include "red_black.h"

                   /* static prototypes for tree operation */
                   /* static functions #{{{ */
                
                /* insertion */
/* enter in a data value, return 1 if success, 0 if failure */
static uint32_t place_data(rbTree *tree, struct_room_info *toAdd);

                /* insertion cases */
/* checks all the cases conforming to the red-black tree algorithm for insert */
static void check_insert_cases(rbTree *tree, rbNode *current);
/* case 1: N is the root node */
static uint32_t insert_case_1(rbNode *node, rbNode *parent);
/* case 2: N's parent is black */
static uint32_t insert_case_2(rbNode *parent);
/* case 3: N's parent and uncle are red */
static uint32_t insert_case_3(rbTree *tree, rbNode *parent, rbNode *grand, rbNode *uncle);
/* case 4: N is added to the to grandparent -> child[LEFT] -> child[RIGHT]
                            OR
           N is added to the grandparent -> child[RIGHT] -> child[LEFT] */
static uint32_t insert_case_4(rbTree *tree, rbNode *node, rbNode *parent, rbNode *grand);
/* case 5: N is added to grandparent -> child[LEFT] -> child[LEFT]
                            OR
          N is added to grandparent -> child[RIGHT] -> child[RIGHT] */
static void insert_case_5(rbTree *tree, rbNode *node, rbNode *parent, rbNode *grand);

                /* deletion */
/* remove the given node from the tree */
static uint32_t remove_node(rbTree *tree, rbNode *match);

/* removes everything from the tree. returns 1 if success, 0 if failure */
static uint32_t remove_rbTree(rbNode **node);

                /* removal cases */
/* case 1: replacement node N is the nwew root */
static void removal_case_1(rbTree *tree, rbNode *node);
/* case 2: S is red */
static void removal_case_2(rbTree *tree, rbNode *node);
/* case 3: parent, sibling, and the siblings children are all black */
static void removal_case_3(rbTree *tree, rbNode *node);
/* case 4: S & S's children are black, parent is red */
static void removal_case_4(rbTree *tree, rbNode *node);
/* case 5: S is black, S's left child is red, right child is black, N is left
   child of its parent. */
static void removal_case_5(rbTree *tree, rbNode *node);
/* case 6: S is black, S's right child is red, N is left child of its parent */
static void removal_case_6(rbTree *tree, rbNode *node);

                /* display */
/* display every nodes data, and its color/flag */
static uint32_t display_rbTree(rbNode *node);

                /* utility functions */
/* count every instance of a piece of data, return count */
static uint32_t count_data(rbNode *node, const char *room_name);
/* return the total ammount of nodes in the rbTree */
static uint32_t count_total(rbNode *node);

               /* red black tree utility */
/* rotate a node left, parent = node -> parent */
static void left_rotate(rbTree *tree, rbNode *node);
/* rotate a node right */
static void right_rotate(rbTree *tree, rbNode *node);
/* retrieve the node that has a match */
static rbNode* retrieve_match(rbNode *node, char *toFind);
/* find in order predecessor, replace data with node, return predecessor */
static rbNode* replace_predecessor(rbNode *node);

                /* end private functions #}}} */


                   /* initializations */

int rbTree_init(rbTree *tree)/*#{{{*/
{
    /* check if tree is allready made for the pointer */
    assert(tree != NULL);

    if(tree == NULL){
        return 0;}
    
    /* initialize root */
    tree -> root = NULL;

    return 1;
} /* end rb_init #}}} */

                   /* insertion */

uint32_t give_data(rbTree *tree, struct_room_info *toAdd)/*#{{{*/
{
    if(!tree)
    {
        noerr_msg("give_data: Tree is NULL");
        return 0;
    }
    
    /* feel place_data could be recursive too */
    return place_data(tree, toAdd);
} /* end give_data #}}} */


uint32_t place_data(rbTree *tree, struct_room_info *toAdd)/*#{{{*/
{
    uint32_t lastTurn;
    int cmp_ret;
    size_t size;
    rbNode *newNode = NULL;   /* holds the located of newest node for checks */
    rbNode *current = NULL;   /* current pointer for traversal */
    rbNode *previous = NULL;  /* previous pointer for traversal */

    current = tree -> root;

    /* traverse to insertion position, previous is parent */
    while(current != NULL) {
        previous = current;
        /* sort by name */
        size = strlen(current->room->room_name) + 1;
        cmp_ret = strncmp(toAdd->room_name, current->room->room_name, size);
        if(cmp_ret < 0) {
            /* go to left child */
            current = current -> child[LEFT];
            lastTurn = LEFT;
        } /* end if */
        else {
            /* go to right child */
            current = current -> child[RIGHT];
            lastTurn = RIGHT;
        } /* end else */
    } /* end while */

/*******************************************************************************
 * TODO: This portion should be its own functions for allocating and filling
 *        the node with data.
 ******************************************************************************/
    /* allocate and set the new node to be added */
    newNode = CALLOC(rbNode);
    if(newNode == NULL) {
        err_msg("give_data: new node failed to alloc");
        return 0;
    } 

    newNode->room = toAdd; 
   
    /* Set the room to the provided pointed to data */
    newNode -> parent = previous;
    newNode -> child[LEFT] = newNode -> child[RIGHT] = NULL;
    newNode -> cflag = RED;

    /* insert the new node into the red black tree data structure */
    /* if previous exists, root must also, else set root */
    (previous != NULL) ? (previous -> child[lastTurn] = newNode)
                       : (tree -> root = newNode);

    /* checks integrity of the tree at the inserted node */
    check_insert_cases(tree, newNode);

    return 1;
} /* end place_data #}}} */

void check_insert_cases(rbTree *tree, rbNode *current)/*#{{{*/
{
    rbNode *grand = NULL;  /* grand of current node */
    rbNode *uncle = NULL;  /* uncle of current node */
    rbNode *parent = NULL; /* parent of current node */
    
    /* initialization */
    parent = current -> parent; /* current will always exist in this implementation */
    grand = find_parent(parent);
    uncle = find_uncle(current, grand);
    
    /* case 1: N is the root node */
    if(insert_case_1(current, parent)){
        return;}

    /* case 2: N's parent is black */
    if(insert_case_2(parent)){
        return;}

    /* case 3: N's parent and uncle are red */
    if(insert_case_3(tree,parent, grand, uncle)){
        return;}

    /* case 4: N is added to the to grandparent -> child[LEFT] -> child[RIGHT]
               N is added to the grandparent -> child[RIGHT] -> child[LEFT] */
    if(insert_case_4(tree, current, parent, grand)){
        return;}

    /* case 5: N is added to grandparent -> child[LEFT] -> child[LEFT]
               N is added to grandparent -> child[RIGHT] -> child[RIGHT] */
    insert_case_5(tree, current, parent, grand);
} /* end check_insert_cases #}}} */

uint32_t insert_case_1(rbNode *node, rbNode *parent)/*#{{{*/
{
    /* if node is top of tree (no parent) */
    if(!parent)
    {
        node -> cflag = BLACK;
        return 1;
    } /* end if */

    /* case was not valid */
    return 0;
} /* end insert_case_1 #}}} */

uint32_t insert_case_2(rbNode *parent)/*#{{{*/
{
    /* if parent is black */
    if(parent && !parent -> cflag){
        return 1;}
    
    /* parent was red */
    return 0;
} /* end insert_case_2 #}}} */

uint32_t insert_case_3(rbTree *tree, rbNode *parent, rbNode *grand, /*#{{{*/
                       rbNode *uncle)
{
    /* if the grand and  uncle exists and its color is red */
    if(uncle && uncle -> cflag)
    {
        parent -> cflag = BLACK;
        uncle -> cflag = BLACK;

        /* if theres an uncle theres a grandparent */
        grand -> cflag = RED;

        check_insert_cases(tree, grand);
        return 1;
    } /* end if */

    return 0;
} /* end insert_case_3 #}}} */

uint32_t insert_case_4(rbTree *tree,   rbNode *node,/*#{{{*/
                       rbNode *parent, rbNode *grand)
{
    if(node == parent -> child[RIGHT] && parent == grand -> child[LEFT])
    {
        /* rotate parent left */
        left_rotate(tree, parent);
        
        /* fix result with case 5 */
        node = node -> child[LEFT];
        insert_case_5(tree, node, node -> parent, node -> parent -> parent);
        return 1;
    } /* end if */

    if(node == parent -> child[LEFT] && parent == grand -> child[RIGHT])
    {
        /* rotate parent right */
        right_rotate(tree, parent);

        /* fix result with case 5 */
        node = node -> child[RIGHT];
        insert_case_5(tree, node, node -> parent, node -> parent -> parent);
        return 1;
    } /* end if */
   
    return 0;
} /* end insert_case_4 #}}} */

void insert_case_5(rbTree *tree  , rbNode *node, /*#{{{*/
                   rbNode *parent, rbNode *grand)
{
    if(node == parent -> child[LEFT] && parent == grand -> child[LEFT])
    {
        /* adjust colors */
        parent -> cflag = BLACK;
        grand -> cflag = RED;

        right_rotate(tree, grand);
    } /* end if */
    else
    {
        assert(node == parent -> child[RIGHT] && parent == grand -> child[RIGHT]);
        /* adjust colors */
        parent -> cflag = BLACK;
        grand -> cflag = RED;

        left_rotate(tree, grand);
   } /* end else */
} /* end insert_case_5 #}}} */


                   /* deletion */

uint32_t remove_first(rbTree *tree, char* toRemove)/*#{{{*/
{
    rbNode *match = NULL;  /* node that matched toRemove */

    if (!tree) {
        noerr_msg("remove_first: tree not initialized, nothing to remove.");
        return 0;
    }

    /* find the node that has matching data, match will be parent -> child */
    match = retrieve_match(tree -> root, toRemove);

    if(remove_node(tree, match))
        return 1;

    return 0;
} /* end remove_first #}}} */

/* OPT NOTE: this function starts from root each time it searchs for the item
 *           to be removed. Much of the tree changes with removal, therefore
 *           this may be a required lag as appose to doing it in 1 search. 
 *             
 *           Just mentioning it off the top of my head. Might be fine.
 */
uint32_t remove_each(rbTree *tree, char *toRemove)/*#{{{*/
{   
    uint32_t removals = 0;

    assert(tree != NULL);
    
    /* while something gets removed, removes all instances of toRemove */
    while(remove_first(tree, toRemove))
          ++removals;

    return removals;
} /* end remove_each #}}} */

void remove_all(rbTree *tree)/*#{{{*/
{
    if(!tree && !tree -> root)
    {
        noerr_msg("remove_all: tree not initialized, nothing to remove.");
        return;
    } /* end if */

    remove_rbTree(&tree -> root);
} /* end remove_all #}}} */

uint32_t remove_rbTree(rbNode **node)/*#{{{*/
{
    if(*node == NULL){
        return 0;}
    
    remove_rbTree(&(*node) -> child[LEFT]);
    remove_rbTree(&(*node) -> child[RIGHT]);

    /* if we are at a leaf, deallocate it */
    if(!(*node) -> child[LEFT] && !(*node) -> child[RIGHT])
    {
        /* free data */
        _room_free_info((*node)->room);
        /* free node */
        free(*node);
        /* set to NULL for parent node to become a leaf */
        *node = NULL;
    } /* end if */

    return 1;
} /* end remove_all #}}} */


                  /* removal cases */

uint32_t remove_node(rbTree *tree, rbNode *match) /*#{{{*/
{
    rbNode *child = NULL; /* childs of node to be free */
    rbNode *pred = NULL;  /* predecessor of node - match */

    if(match == NULL){
        return 0;} /* no match to find */

    if(match -> child[LEFT] != NULL && match -> child[RIGHT] != NULL)
    {
        /* find in order predecessor and replace the data in node, gets nodes
           address */
        pred = replace_predecessor(match);
        match = pred;
    } /* end if */

    assert(match -> child[LEFT] == NULL || match -> child[RIGHT] == NULL);
    child = (match -> child[RIGHT] == NULL) ? match -> child[LEFT] : match -> child[RIGHT];

    if(match -> cflag == BLACK)
    {
        match -> cflag = (child == NULL) ? BLACK : child -> cflag;
        removal_case_1(tree, match);
    } /* end if */
    
    /* replace match (to free) with its child */
    if(match -> parent == NULL){
        tree -> root = child;} /* end if */
    else
    {
        if(match == match -> parent -> child[LEFT]){
            match -> parent -> child[LEFT] = child;}  /* end if */
        else{
            match -> parent -> child[RIGHT] = child;} /* end else */
    } /* end else */

    if(child != NULL){
        child -> parent = match -> parent;}
    
    /* ensure root is black */
    if(match -> parent == NULL && child != NULL){
        child -> cflag = BLACK;}
    
    _room_free_info(match->room);
    free(match);

    return 1;
} /* end remove_node #}}} */

void removal_case_1(rbTree *tree, rbNode *node)/*#{{{*/ 
{
    /* nodes parent is NULL */
    if(node -> parent != NULL){
        removal_case_2(tree, node);}
} /* end removal_case_1 #}}} */

void removal_case_2(rbTree *tree, rbNode *node)/*#{{{*/
{
    rbNode *sibling = NULL;
    sibling = find_sibling(node);
    
    /* if sibling is red */
    if(sibling -> cflag == RED)
    {
        node -> parent -> cflag = RED;
        sibling -> cflag = BLACK;
        if(node == node -> parent -> child[LEFT]){
            left_rotate(tree, node -> parent);}  /* end if */
        else{
            right_rotate(tree, node -> parent);} /* end else */
    } /* end if */

    removal_case_3(tree, node);
} /* end removal_case_2 #}}} */

void removal_case_3(rbTree *tree, rbNode *node)/*#{{{*/
{
    rbNode *sibling = find_sibling(node);
    int32_t cLeft = find_rbcolor(sibling -> child[LEFT]);
    int32_t cRight = find_rbcolor(sibling -> child[RIGHT]);

    if( (node -> parent -> cflag == BLACK) &&
        (sibling -> cflag == BLACK) &&
        (cLeft == BLACK) &&
        (cRight == BLACK))
    { /* start if */
        sibling -> cflag = RED;
        removal_case_1(tree, node -> parent);
    } /* end if */
    else
    {
        removal_case_4(tree, node);
    } /* end else */
} /* end removal_case_3 #}}} */

void removal_case_4(rbTree *tree, rbNode *node)/*#{{{*/
{
    rbNode *sibling = find_sibling(node);
    int32_t cLeft = find_rbcolor(sibling -> child[LEFT]);
    int32_t cRight = find_rbcolor(sibling -> child[RIGHT]);
    
    if( (node -> parent -> cflag == RED) &&
        (sibling -> cflag == BLACK) &&
        (cLeft == BLACK) &&
        (cRight == BLACK))
    { /* start if */
        sibling -> cflag = RED;
        node -> parent -> cflag = BLACK;
    } /* end if */
    else
    {
        removal_case_5(tree, node);
    } /* end else */
} /* end removal_case_4 #}}} */

void removal_case_5(rbTree *tree, rbNode *node)/*#{{{*/
{
    rbNode *sibling = find_sibling(node);
    int32_t cLeft = find_rbcolor(sibling -> child[LEFT]);
    int32_t cRight = find_rbcolor(sibling -> child[RIGHT]);

    if(sibling -> cflag == BLACK)
    {
        if( (node == node -> parent -> child[LEFT]) &&
            (cRight == BLACK) &&
            (cLeft == RED)) 
        { /* start if */
            sibling -> cflag = RED;
            sibling -> child[LEFT] -> cflag = BLACK;
            right_rotate(tree, sibling);
        } /* end if */
        else
        {
            if( (node == node -> parent -> child[RIGHT]) &&
                (cLeft == BLACK) &&
                (cRight == RED))
            { /* start if */
                sibling -> cflag = RED;
                sibling -> child[RIGHT] -> cflag = BLACK;
                left_rotate(tree, sibling);
            } /* end if */
        } /* end else */
    } /* end if */

    removal_case_6(tree, node);
} /* end removal_case_5 #}}} */

void removal_case_6(rbTree *tree, rbNode *node)/*#{{{*/
{
    rbNode *sibling = find_sibling(node);

    sibling -> cflag = node -> parent -> cflag;
    node -> parent -> cflag = BLACK;

    if(node == node -> parent -> child[LEFT])
    {
        assert(sibling -> child[RIGHT] -> cflag == RED);

        sibling -> child[RIGHT] -> cflag = BLACK;
        left_rotate(tree, node -> parent);
    }
    else
    {
        assert(sibling -> child[LEFT] -> cflag == RED);

        sibling -> child[LEFT] -> cflag = BLACK;
        right_rotate(tree, node -> parent);
    } /* end else */
} /* end removal_case_6 #}}} */


                 /* utility */

uint32_t data_count(rbTree *tree, const char *room_name)/*#{{{*/
{
    assert(tree != NULL);
    return count_data(tree -> root ,room_name);
} /* end data_count #}}} */

uint32_t count_data(rbNode *node, const char *room_name)/*#{{{*/
{
    uint32_t match; /* total times match was found */
    size_t len;

    if (node == NULL)
        return 0;

    len = strlen(node->room->room_name) + 1; 
    /* check if current nodes data is a match */
    if (!(strncmp(room_name, node->room->room_name, len)))
        match = 1;
    else
        match = 0;

    return count_data(node -> child[RIGHT], room_name) 
           + count_data(node -> child[LEFT], room_name) + match;
} /* end count_data #}}} */

uint32_t total_data_count(rbTree *tree)/*#{{{*/
{
    if(!tree || !tree -> root)
        return 0;

    return count_total(tree -> root);
} /* end total_data_count #}}} */

uint32_t count_total(rbNode *node)/*#{{{*/
{
    if(!node){
        return 0;}

    return count_total(node -> child[LEFT]) + count_total(node -> child[RIGHT]) 
           + 1;
} /* end count_total #}}} */

inline void left_rotate(rbTree *tree, rbNode *node)/*#{{{*/
{
    /* save nodes right, and nodes parent */
    rbNode *sRight = node -> child[RIGHT];
    rbNode *nParent = node -> parent;

    /* fix root */
    if(node -> parent == NULL){
        tree -> root = sRight;}
    
    /* rotate */
    node -> child[RIGHT] = sRight -> child[LEFT];
    sRight -> child[LEFT] = node;

    /* adjust nparent accordingly */
    if(nParent)
    {
        if(nParent -> child[LEFT] == node){
            nParent -> child[LEFT] = sRight;}  /* end if */
        else{
            nParent -> child[RIGHT] = sRight;} /* end else */
    } /* end if */

    /* fix parents */
    sRight -> parent = nParent;
    node -> parent = sRight;

    if(node -> child[RIGHT] != NULL){
        node -> child[RIGHT] -> parent = node;}
} /* end left_rotate #}}} */

inline void right_rotate(rbTree *tree, rbNode *node)/*#{{{*/
{
    /* save nodes left, and nodes parent */
    rbNode *sLeft = node -> child[LEFT];
    rbNode *nParent = node -> parent;
    
    /* fix root */
    if(node -> parent == NULL){
        tree -> root = sLeft;} 

    /* rotate */
    node -> child[LEFT] = sLeft -> child[RIGHT];
    sLeft -> child[RIGHT] = node;

    /* adjust nparent accordingly */
    if(nParent)
    {
        if(nParent -> child[LEFT] == node){
            nParent -> child[LEFT] = sLeft;}  /* end if */
        else{
            nParent -> child[RIGHT] = sLeft;} /* end else */
    } /* end if */

    /* fix parents */
    sLeft -> parent = nParent;
    node -> parent = sLeft;
    if(node -> child[LEFT] != NULL)
        node -> child[LEFT] -> parent = node;
} /* end right_rotate #}}} */

struct_room_info* find_match(rbTree *tree, char *toFind)/*#{{{*/
{
    rbNode *match;
    assert(tree != NULL);
    match = retrieve_match(tree->root, toFind);
    if (!match)
        return NULL;
    return match->room;
} /* end find_place #}}} */

rbNode* retrieve_match(rbNode *node, char *toFind)/*#{{{*/
{
    int cmp_ret;
    int len;
    if(node == NULL)
        return NULL;
  
    len = strlen(node->room->room_name) + 1;
    cmp_ret = strncmp(toFind, node->room->room_name, len);
    if(cmp_ret == 0)
        return node;

    if(cmp_ret < 0)
        return retrieve_match(node -> child[LEFT], toFind);
    else
        return retrieve_match(node -> child[RIGHT], toFind);
} /* end find_place #}}} */
                
rbNode* replace_predecessor(rbNode *node)/*#{{{*/
{
    rbNode *current = NULL;

    assert(node != NULL);

    current = node -> child[LEFT];

    if (current) {

        /* get the predecessor */
        while (current -> child[RIGHT] != NULL)
            current = current -> child[RIGHT];
        
        
        /* free old data */
        _room_free_info(node->room);
        /* aquire copy of new data */
        node->room = _room_get_copy(node->room);

        return current;
    }

    return node;
} /* end replace_predecessor #}}} */
           
                /* display */

uint32_t display_all(rbTree *tree)/*#{{{*/
{
    if (!tree || !tree -> root) {
        noerr_msg("display_all: tree is empty, nothing to display");
        return 0;
    }

    return display_rbTree(tree -> root);
} /* end display_all #}}} */

uint32_t display_rbTree(rbNode *node)/*#{{{*/
{
    if(node == NULL)
        return 0;

    display_rbTree(node -> child[LEFT]);

    /* TODO: make a print room info function and call it here. */
    printf("room struct address: %p, flag: %d, parent room addr: %p\n", 
            (void*)node->room, node->cflag, 
           ((node -> parent) ? (void*)node -> parent -> room : 0));

    display_rbTree(node -> child[RIGHT]);

    return 1;
} /* end display_all #}}} */
/******** EOF *********/
