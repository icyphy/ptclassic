/*
   CLISocket.c
   Demonstration of using socket routines for communication
   between Synopsys VHDL simulator and C.
   Version:  $Id$
*/

#include "CLISocket.h"
#include <sys/errno.h>

extern int errno;

/*****************
   Socket creation
*****************/
void create_socket(idata)
     INSTANCE_DATA *idata;
{
  idata->nearsock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (idata->nearsock == -1)
    {
      perror(idata->dummy);
    }
  idata->nearaddr.sa_family = AF_UNIX;
  idata->faraddr.sa_family = AF_UNIX;
  (void) strncpy(idata->nearaddr.sa_data, idata->nearstring,
	  idata->nearnamelen+1);
  (void) strncpy(idata->faraddr.sa_data, idata->farstring,
	  idata->farnamelen+1);
  (void) unlink(idata->nearaddr.sa_data);
  return;
}

/**********************
   Establish connection
**********************/
int connect_socket(idata)
     INSTANCE_DATA *idata;
{
  int printflag = 1;
  int status = 0;

  status = connect(idata->nearsock, &idata->faraddr,
	  idata->faraddrlen);
/*
  delay(1000000000);
*/
  if(status < 0) {
    /* If file not found, may just need to wait a bit
       so don't signal an error for ENOENT, and don't
       reset the print flag for ENOENT. */
    if(errno == ENOENT) {
      return(status);
    }
    /* If connection refused, need to close down this
       socket as it may be garbled, and open a new one,
       waiting to attempt to connect again. */
    if(errno == ECONNREFUSED) {
      perror(idata->dummy);
      (void) close_socket(idata);
      (void) create_socket(idata);
      return(status);
    }
    if(printflag) {
/*
PERRNO;
*/
      perror(idata->dummy);
      printflag = 0;
    }
  }
  return(status);
}

/**********
   Get Data
**********/
void get_data(idata)
     INSTANCE_DATA *idata;
{
  int printflag = 1;
  int readstat = 0;

  readstat = read(idata->nearsock, idata->buffer, idata->nbytes);
  while(readstat == 0)
    {
      if(printflag) {
/*
	(void) printf("nothing to read\n");
*/
	printflag = 0;
      }
      readstat = read(idata->nearsock, idata->buffer, idata->nbytes);
    }
  if(readstat < 0) {
    /* If connection reset by peer, it is likely that
       it is because it has no more data to send, and
       that this side is trying to read more data. */
    if(errno == ECONNRESET) {
      return;
    }
    perror(idata->dummy);
  }

  return;
}

/**********
   Put Data
**********/
void put_data(idata)
     INSTANCE_DATA *idata;
{
  if((write(idata->nearsock, idata->buffer, idata->nbytes)) < 0) {
    /* If pipe is broken, it is likely that
       it is because the CGC side has concluded operations
       and that this side is trying to write more data. */
    if(errno == EPIPE) {
      return;
    }
    perror(idata->dummy);
  }

  return;
}

/**************
   Close socket
**************/
void close_socket(idata)
     INSTANCE_DATA *idata;
{
  (void) shutdown(idata->nearsock,2);
  (void) close(idata->nearsock);
  (void) unlink(idata->nearaddr.sa_data);
  return;
}
