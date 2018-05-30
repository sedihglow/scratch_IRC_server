/*******************************************************************************
 * Filename: irc_client.c
 *
 * Written by: James Ross
 ******************************************************************************/

#include "irc_client.h"

static volatile bool g_serv_crashed = false;
static volatile bool g_user_logout  = false;


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

    ret = strncmp(input, "a ", 2);
    if (ret == 0)
        return RC_FA;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_FL;

    ret = strncmp(input, "r ", 2);
    if (ret == 0)
        return RC_FR;

    return RC_ERR;
} /* end find_fcmd */


static int find_room_cmd(char *input)
{
    int ret;
    
    /* TODO Single letter shit needs some if statments before it to check for newline
     * or null to make sure it is valid and not just lifr or asda
     *
     * Basically all the shit after /<shit> <arg parsing>
     */
    ret = strncmp(input, "l", 1);
    if (ret == 0)
        return RC_RL;
      
    return RC_ERR;
} /* end find_room_cmd */
/* end static parse helpers */


/*******************************************************************************
 *                          Header Functions
 ******************************************************************************/


/*******************************************************************************
 * Parse's the input of the client. Finds what type of input it was, msg or 
 * command then which command if applicable.
 ******************************************************************************/
int irc_handle_user_input(struct_irc_info *irc_info, char *input)
{
    int ret, type, len;
    char *cli_name = irc_info->client->name;
    
    if (input[0] != '/') {
        if (strcmp(irc_info->client->rooms[0]->room_name, R_DFLT_ROOM) != 0)
            return SUCCESS; /* ignore the message, in the void */

        return com_send_chat_message(cli_name, input, irc_info->serv_info);
    }

    /* /f [a, r, l] */
    ret = strncmp(input, "/f ", 3); 
    if (ret == 0) {
        type = find_fcmd(input+3);
        if (type == RC_FL)
            ret = cli_handle_flist(type, irc_info->client, NULL);
        else if (type == RC_FA || type == RC_FR)
            ret = cli_handle_flist(type, irc_info->client, input+5);
        else
            return FAILURE; /* invalid command */
        return ret;
    }

    /* /join */
    len = sizeof(JOIN); /* includes '\0' */
    ret = strncmp(input, JOIN, len-1);
    if (ret == 0) {
        if (input[len-1] == ' ') { /* if true, check next index for '\0' */
            if (input[len] != '\0') {
                ret = com_send_join_message(cli_name, input+len, 
                                            irc_info->serv_info);
                return ret;
            } else {
                return FAILURE; /* invalid command */
            }
        } else {
            return FAILURE;
        }
    }

    /* /leave */
    len = sizeof(LEAVE);
    ret = strncmp(input, LEAVE, len-1);
    if (ret == 0) {
        if (input[len-1] == ' ') {         /* leave specified room */
            ret = com_send_leave_message(cli_name, input+len, 
                                         irc_info->serv_info);
        } else if (input[len-1] == '\0') { /* leave current room */
            ret = com_send_leave_message(cli_name,
                  irc_info->client->rooms[irc_info->client->current_r]->room_name,
                  irc_info->serv_info);
        } else {
            return FAILURE; /* invalid command */
        }
        return ret;
    }

    /* /exit */
    len = sizeof(EXIT_IRC);
    ret = strncmp(input, EXIT_IRC, len-1);
    if (ret == 0) {
        if (input[len-1] == '\0') {
            ret = com_send_exit_message(cli_name, irc_info->serv_info);
            if (ret == FAILURE)
                return FAILURE;
            return RC_EXIT; 
        } else {
            return FAILURE;
        }
    }

    ret = strncmp(input, ROOM_L, sizeof(ROOM_L)-1);
    if (ret == 0) {
    //    find_room_cmd(input+5);
    }

    ret = strncmp(input, PRIV_MSG, sizeof(PRIV_MSG)-1);
    if (ret == 0) {
    //    RC_PM;
    }

    /* This needs to be AFTER comparing ROOM_L since both start with /r */
    ret = strncmp(input, PRIV_REP, sizeof(PRIV_REP)-1);
    if (ret == 0) {
    //    RC_PR;
    }

    ret = strncmp(input, VOID, sizeof(PRIV_REP)-1);
    if (ret == 0) {
    //    RC_VOID;
    }

    if (strcmp(irc_info->client->rooms[0]->room_name, R_DFLT_ROOM) != 0)
        return SUCCESS; /* ignore the message, in the void */

    return com_send_chat_message(irc_info->client->name, input,
                                 irc_info->serv_info);
} /* end irc_handle_user_input */



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
        if (len <= CLI_NAME_MAX && len > 1) {

            /* inform the server of the clients name and register */
            ret = com_send_logon_message(input, irc_info->serv_info);
            if (_usrLikely(ret == SUCCESS)) {

                /* wait until server responds confirming validity. */
                ret = com_get_logon_result(irc_info->serv_info->sockfd);
                if (_usrLikely(ret == SUCCESS)) {

                    /* logon successful, set local client information,
                     * place in default room */
                    cli_set_new_cli_info(irc_info->client, input);
                    printf("\ncongradulations, logon Sucecssful.\n");

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
     } while (len > CLI_NAME_MAX && len > 1);
} /* end display_logon */

void display_room_welcome(char *room_name, int num_users)
{
    int ret;

    ret = strcmp(room_name, R_DFLT_ROOM);
    if (ret == 0) {
        printf("\nYou have entered the %s, You are in silence.\n"
               "Number of users in the room: 0\n\n", room_name);
        return;
    }

    printf("\nWelcome to the room  %s, currently with %d active users.\n\n",
            room_name, num_users);
} /* end display_room_welcome */


int irc_exec_client_response(struct_irc_info *irc_info, 
                             struct_serv_message *serv_msg)
{
    struct_cli_info *cli = irc_info->client;

    switch (serv_msg->type) {

    case RC_MSG:
         
    break;
    case RC_JOIN: /* format: type | 0/1 | \r */
    if (strcmp(cli->rooms->room_name, R_DFLT_ROOM) == 0) {
        /* leave the void */
        room_free_info(cli->rooms[0]);
        cli->rooms[0] = NULL;
    }
    
    if ((uint8_t)(serv_msg->msg[0]) == 0)
        return FAILURE;

    break;

    case RC_LEAVE:

    break;

    case RC_EXIT:

    break;

    default:
        err_msg("Invalid command type recieved");
        return FAILURE;
    } 

    return SUCCESS;
} /* end irc_exec_client_response */


void* irc_handle_server_requests(void *args)
{
    struct_irc_info *irc_info = (struct_irc_info*) args;
    uint8_t rx[_COM_IO_BUFF] = {'\0'};
    struct_serv_message *serv_msg;

    for ( ;; ) {
        /* block waiting for message from server. */
        receive_from_server(irc_info->serv_info->sockfd, rx, _COM_IO_BUFF, 
                            NO_FLAGS);
        if (rx[0] == 0x0 || rx[0] == RC_EXIT) { 
            printf("Server has crashed. Exiting program.\n");
            /* server crashed or i have a bug */
            g_serv_crashed = true;
            pthread_exit(NULL);
        }

        serv_msg = com_parse_server_msg(rx);
        if (!serv_msg)
            errExit("irc_handle_server_request: serv_msg parse failure.");
       
        irc_exec_client_response(irc_info, serv_msg);
         
    }

    pthread_exit(NULL);
} /* end handle_server_requests() */

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
    /* waits for server response untill successful logon. */
    irc_logon_client(irc_info); 
    display_clear();
    display_room_welcome(R_DFLT_ROOM, 0);

    /* Spin up recv && exec thread */
    ret = pthread_create(&t_id, NULL, irc_handle_server_requests, (void*)irc_info);
    if (ret)
        errnumExit(ret, "irc_client: Failed to create recv thread.");

    for ( ;; ) { 
        input = irc_get_user_input();

        if (g_serv_crashed == true) {
            pthread_join(t_id, NULL); /* wait for server response and thread close */
            free(input);
            break;
        }

        ret = irc_handle_user_input(irc_info, input);

        if (ret == FAILURE) {
            printf("- INVALID COMMAND -");
        } else if (ret == RC_EXIT || g_serv_crashed == true) {
            pthread_join(t_id, NULL); /* wait for server response and thread close */
            free(input);
            break;
        }
        free(input);
    }

    irc_free_info(irc_info);
} /* end irc_client() */
/****** EOF ******/

#if 0
/* TODO: These might not get implemented due to time. Clean up final version. */
static int find_bcmd(char *input)
{
    int ret;

    ret = strncmp(input, "l ", 2);
    if (ret == 0)
        return RC_BL;

    ret = strncmp(input, "a", 2);
    if (ret == 0)
        return RC_BA;

    ret = strncmp(input, "r ", 2);
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

    return RC_ERR;
}
#endif
