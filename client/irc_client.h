#include "client.h"

#define SERV_ADDR   "131.252.208.085" /* linux.cecs.pdx.edu */
#define SERV_LEN    16                /* Fits IPV4 */
#define SERV_PORT   31000             /* port listening on server */
#define NET_DOMAIN  AF_INET           /* network domain we are using. IPV4 */
#define IP_PROTOCOL 0                 /* Default for type in socket() */

/* Function executed by main to run the irc client */
void irc_client(void);
