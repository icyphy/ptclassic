/*
   CLISocket.h
   Demonstration of using socket routines for communication
   between Synopsys VHDL simulator and C.
   Version:  $Id$
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ddi.h>

/* Define SOCK_BASE_NAME */
#include "CLIConstants.h"

void create_socket();
int connect_socket();
void get_data();
void put_data();
void close_socket();

#define	CHECK_ERRNO	if(cli_warnings || cli_errno) { (void) printf("file %s, line %d\n",__FILE__,__LINE__); cliPrintError((char *)0); exit(1) ;}

#define	PERRNO	{ (void) printf("file %s, line %d\n",__FILE__,__LINE__); }

/***************************
   Data Structure Definition
***************************/
typedef struct {
  cliGID gidPairid;	/* ID for generic "pairid" */
  cliGID gidNumxfer;	/* ID for generic "numxfer" */
  cliPID pidGo;		/* Pin ID for "go" */
  cliPID pidData;	/* Pin ID for "data" */
  cliPID pidDone;	/* Pin ID for "done" */

  struct sockaddr_un nearaddr, faraddr;
  int nearsock;
  char buffer[256];
  char *dummy;
/*
Due to the limits on struct sockaddr, anything longer
than 14 characters is too long.  We leave extra characters
in the array to allow for null terminators, margin, etc.
Hence, array size of 16.

But with struct sockaddr_un, you can have a path up to 108 chars.
*/
/*
  char nearstring[16];
  char farstring[16];
*/
  char nearstring[108];
  char farstring[108];
  char format[16];
  int nearnamelen;
  int farnamelen;
  int faraddrlen;
  int intptr;
  double realptr;
  int nbytes;
  int connection_established;
  int data_requested;
  int pairid;
  int numxfer;
} INSTANCE_DATA;
