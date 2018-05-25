/*******************************************************************************
 * Filename: irc_client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_client.h"


/******************************************************************************* 
 *                          Static Functions
 ******************************************************************************/


/*******************************************************************************
 * TODO: NOTE: Aware of magic numbrs and redundant compares copied into other
 *             functions such as "add ", "a ",etc,etc. Not sure what to do bout
 *             it that would be any more efficient.
 *
 *             Maybe make them each an inline? :/ w/e
 *
 *
 *             Also the magic strings.
 ******************************************************************************/

/* Start static parse helpers */
static int find_fcmd(char *input)
{
    int ret;

    ret = strncmp(input, "add ", 4);
    if (ret == 0)
        return RC_FA;

    ret = strncmp(input, "a ", 2);
    if (ret == 0)
        return RC_FA;

    ret = strncmp(input, "list ", 5);
    if (ret == 0)
        return RC_FL;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_FL;

    ret = strncmp(input, "remove ", 7);
    if (ret == 0)
        return RC_FR;

    ret = strncmp(input, "r ", 2);
    if (ret == 0)
        return RC_FR;

    return RC_ERR;
} /* end find_fcmd */

static int find_bcmd(char *input)
{
    int ret;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_BL;

    ret = strncmp(input, "list ", 5);
    if (ret == 0)
        return RC_BL;

    ret = strncmp(input, "a ", 2);
    if (ret == 0)
        return RC_BA;

    ret = strncmp(input, "add ", 4);
    if (ret == 0)
        return RC_BA;

    ret = strncmp(input, "r ", 2);
    if (ret == 0)
        return RC_BR;

    ret = strncmp(input, "remove ", 6);
    if (ret == 0)
        return RC_BR;

    return RC_ERR;
} /* end find_bcmd */

static int find_inv_cmd(char *input)
{
    int ret;

    ret = strncmp(input, "inv ", 4);
    if (ret == 0)
        return RC_INV;

    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;

    return RC_ERR;
}

static int find_room_cmd(char *input)
{
    int ret;

    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;
      
    ret = strncmp(input, "invite ", 7);
    if (ret == 0)
        return RC_INV;

    return RC_ERR;
} /* end find_room_cmd */
/* end static parse helpers */


/*******************************************************************************
 *                          Header Functions
 ******************************************************************************/


/*******************************************************************************
 * Parse's the input of the client. Finds what type of input it was, msg or 
 * command then which command if applicable.
 *
 * Returns identifier showing which type of input it was.
 *
 * Some functions pass an incrented address so we dont compare the same shit
 * twice. Only one that doesnt in /inv vs /invite since there is no space
 * seperator.
 *
 * TODO: This might be threaded and called after input, on the thread that runs 
 *       the input waiting. Then pass this return value to main thread.
 ******************************************************************************/
int parse_args(char *input)
{
    int ret = 0;

    if (input[0] != '/')
        return RC_MSG;

    ret = strncmp(input, "/f ", 3);
    if (ret == 0)
        return find_fcmd(input+3);

    ret = strncmp(input, "/b ", 3);
    if (ret == 0)
        return find_bcmd(input+3);

    ret = strncmp(input, WHO, sizeof(WHO)-1);
    if (ret == 0)
        return RC_WHO;

    ret = strncmp(input, JOIN, sizeof(JOIN)-1);
    if (ret == 0)
        return RC_JOIN;

    ret = strncmp(input, LOG_OUT, sizeof(LOG_OUT)-1);
    if (ret == 0)
        return RC_LOGOUT;

    ret = strncmp(input, INV, sizeof(INV)-1);
    if (ret == 0)
        return find_inv_cmd(input);

    ret = strncmp(input, ROOM_L, sizeof(ROOM_L)-1);
    if (ret == 0)
        return find_room_cmd(input+5);

    ret = strncmp(input, PRIV_MSG, sizeof(PRIV_MSG)-1);
    if (ret == 0)
        return RC_PM;

    /* This needs to be AFTER comparing ROOM_L since both start with /r */
    ret = strncmp(input, PRIV_REP, sizeof(PRIV_REP)-1);
    if (ret == 0)
        return RC_PR;

    ret = strncmp(input, VOID, sizeof(PRIV_REP)-1);
    if (ret == 0)
        return RC_VOID;

    ret = strncmp(input, EXIT_IRC, sizeof(EXIT_IRC)-1);
    if (ret == 0)
        return RC_EXIT;


    return RC_MSG;
} /* end parse_args */

void* irc_handle_server_requests(void *args)
{
    struct_irc_info *irc_info = (struct_irc_info*) args;

    pthread_exit(NULL);
} /* end handle_server_requests() */

/******************************************************************************
 * Allocate all members and sub-members of struct to_init.
 *
 * A new struct will be allocated and its pointer will be returned.
 *
 * Returns NULL on error.
 ******************************************************************************/
struct_irc_info* init_irc_info(void)
{
    struct_irc_info *init;
    
    init = CALLOC(struct_irc_info);
    if(!init)
        return NULL;

    init->client    = cli_init_info();
    if (!init->client)
        return NULL;
    init->serv_info = com_init_serv_info();
    init->tx        = com_init_io_ring();
    init->rx        = com_init_io_ring();
    return init;
} /* end init_irc_info() */

void irc_free_info(struct_irc_info *dest)
{
    cli_free_info(dest->client);
    com_free_serv_info(dest->serv_info);
    com_free_io_ring(dest->tx);
    com_free_io_ring(dest->rx);
    free(dest);
} /* end irc_free_info() */

/*******************************************************************************
 * TODO: I dont like how this works with the buffer sizes n such. *shrug*
 ******************************************************************************/
char* irc_get_user_input(void)
{
    char *input;
    char buff[IO_STR_LEN_MAX] = {'\0'};
    ssize_t rbytes = 0;
    size_t len;

    READ_INPUT(STDIN_FILENO, buff, IO_STR_LEN_MAX-1, rbytes);
   
    /* check for newline */
    len = strlen(buff);
    if (buff[len-1] == '\n') {
        printf (" newline found\n");
        buff[len-1] = '\0';
    } else {
        RD_CLR_STDIN();
        return NULL;
    }

    /* rbytes should be at least a newline here tho. */
    if (rbytes == 0)
        return NULL;
    
    /* remove newline and replace with NULL, otherwise just makes last '\0' */
    buff[rbytes-1] = '\0';
    
    input = CALLOC_ARRAY(char, rbytes); 
    strncpy(input, buff, rbytes);

    return input; 
} /* end get_user_input() */

void display_clear(void)
{
    char newline[100] = {'\n'};
    printf("%s",newline);
} /* end display_clear */

void display_welcome(void)
{
    display_clear();
    printf("-------- Welcome to Chat --------\n\n"
           "Currently, when choosing a username, it must not be taken.\n"
           "If your desired username is taken, re-enter an alternative.\n\n"
           );
} /* end display_welcome */

void irc_logon_client(struct_irc_info *irc_info)
{
    int ret;
    int servfd;
    size_t len;
    char *input;

    if (irc_info == NULL)
        errExit("irc_logon_client: Initialize things before you get here.");

    /* loop until proper input of username */
    do {
        /* TODO: put in a way to exit. Ask to retry or not */
        fprintf(stdout, "Please enter a username (10 character max): ");
        fflush(stdout);
        input = irc_get_user_input();
        if (input == NULL)
            errExit("irc_logon_client: strange, nothing read...");
        
        len = strlen(input) + 1;
        if (len <= CLI_NAME_MAX) {

            /* inform the server of the clients name and register */
            ret = com_send_logon_message(input, irc_info->serv_info);
            if (_usrLikely(ret == SUCCESS)) {

                /* wait until server responds confirming validity. */
                ret = com_get_logon_result(irc_info->serv_info->sockfd);
                if (_usrLikely(ret == SUCCESS)) {

                    /* logon successful, set local client information, */

                    /* place in the void. */


                } else {
                    printf("Name is already taken, please try again.\n");
                } // end if-else
            } else {
                printf("Failed to transmit, please retry.\n"); 
            } // end if-else
        } else {
            printf("Username too long, max 10 characters.\n");
        } // end if-else

        FREE_ALL(input);
        /* while response is denied || len */
     } while (len > CLI_NAME_MAX);
    /* TODO: On success, make sure to fill in personal client pointer */



} /* end display_logon */

/* Main execution function. Leaves this leaves the program. */
void irc_client(void)
{
    struct_irc_info *irc_info; 
    int ret;
    int current_cmd;
    char *input; /* input is likely to be handled in a thread. */
    bool debug_on = false; /* definition is debug.h, should be an argv. */
    
    /* thread information */
    pthread_t t_id;

    irc_info = init_irc_info();
    if (!irc_info)
        errExit("irc_client: init_irc_info failed.");

    init_client_comm(irc_info->serv_info);

    display_welcome();
    irc_logon_client(irc_info);

//    send_to_server(irc_info->serv_info->sockfd, "james\0\x11\r", sizeof("james\0\x11\r"), 0);

#if 0
    /* Spin up recv && exec thread */
    ret = pthread_create(&t_id, NULL, handle_server_requests, (void*)irc_info);
    if (ret)
        errnumExit(ret, "irc_client: Failed to create recv thread.");

    ret = pthread_detach(t_id); /* i shouldnt need to ret anything */
    if (ret)
        errnumExit(ret, "irc_client: Failed to detach recv thread.");
#endif

    /* TODO: Figure out exit flag after you figure out getting input from user */
    for ( ;; ) { }

#if 0
        input = irc_get_user_input();
        if (input == NULL) {
            noerr_msg("irc_client: strange, nothing read...");
            continue;
        }
        current_cmd = parse_args(input);
    }

    printf("after send");
    
    if (input)
        free(input);
#endif
    irc_free_info(irc_info);
} /* end irc_client() */
/****** EOF ******/
