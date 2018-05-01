/*******************************************************************************
 * Filename: irc_server.h
 *
 *
 * Written by: James Ross
 ******************************************************************************/

#include "server.h"
#include "irc_comm.h" // TODO: Until server scope is adjusted

#define SERV_ADDR   _COM_SERV_ADDR
#define SERV_LEN    _COM_SERV_LEN    
#define SERV_PORT   _COM_SERV_PORT   
#define NET_DOMAIN  _COM_NET_DOMAIN  
#define SOCK_TYPE   _COM_SOCK_TYPE   
#define IP_PROTOCOL _COM_IP_PROTOCOL 
#define IO_BUFF     _COM_IO_BUFF 
 

void irc_server(void);
