#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"
#include "utility.h"

/*
 * network routines for the application
 *
 * Rick L Spickelmier, 11/11/86, last modified 3/2/87
 */

/* kludge - due to moving target, handled in -I on compile */
/* KERNEL define to stop recursive inclusion of time.h in BSD and SUN */
#ifdef sun
#define KERNEL			/* On a sun, define KERNEL */
#include <time.h>
#undef KERNEL
#else				/* Everybody else includes it straight */
#include <time.h>   
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

extern int errno;

/* #include "list.h"    */
/* #include "message.h" */
#include "rpcApp.h"
#include "oh.h"
#incluce "appNet.h"
#include "ansi.h"

EXTERN rpcInternalStatus RPCReceiveFunctionToken
  ARGS((long *value, STREAM stream));
rpcStatus RPCApplicationFunctionComplete();


/* see if the application is byte swapped relative to the server */
void
RPCByteSwappedApplication(a, b, c, d, server)
int a, b, c, d, server;
{
    char number[4];
    long *pointer;
    

    number[0] = a;
    number[1] = b;
    number[2] = c;
    number[3] = d;
    pointer = (long *) number;
    
    if (*pointer != server) {
	RPCByteSwapped = 1;
    } else {
	RPCByteSwapped = 0;
    }
}


int
inet_conn(host, port, option)
char *host;
int port;
int option;
/*
 * make an inet connection, just like dnet_conn
 *
 * host is the name of the host to connect to
 * option is SOCK_STREAM
 *
 * returns socket (>= 0) on success and -1 on failure
 *
 */
{
    int s;
    struct hostent *hp;
    struct sockaddr_in sin;

    (void) memset((char *) &sin, 0, sizeof(struct sockaddr_in));
    if ((hp = gethostbyname(host)) == 0) {
	(void) fprintf(stderr, "RPC Error: bad host: %s\n", host);
	return -1;
    }
    
    sin.sin_family = hp->h_addrtype;  /* better always be AF_INET */

    if (!RPCByteSwapped) {
	sin.sin_port = port;
    } else {
	sin.sin_port = (0x00ff & (port >> 8)) | ((port & 0x00ff) << 8);
    }

    sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    /* memcpy(hp->h_addr, (char *) &sin.sin_addr, hp->h_length); */
    
    if ((s = socket(AF_INET, option, 0)) < 0) {
	(void) fprintf(stderr, "RPC Error: bad inet socket return\n");
	return -1;
    }
    
    if (connect(s, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
	(void) fprintf(stderr, "RPC Error: bad inet connect\n");
	perror("inet connection");
	return -1;
    }

    return s;
}


/*
 * make a connection to a server
 *  connect to a remote port
 *
 * Arguments:
 *   host	name of the remote host
 *   port	port to connect to
 *   protocol   protocol to use ("decnet" or "inet")
 *   sendStream	pointer to the sending stream
 *   receiveStream	pointer to the receiving stream
 *
 * Returns:
 *   RPC_OK	if the connection was made
 *   RPC_ERROR	if anything went wrong
 */
rpcStatus
RPCConnectToServer(host, port, protocol, sendStream, receiveStream)
char *host;
int port;
char *protocol;
STREAM *sendStream;
STREAM *receiveStream;
{
    int s;

    RPCHostName = RPCstrsave(host);
    RPCProtocol = RPCstrsave(protocol);

    if (strcmp(protocol, "inet") == 0) {
	if ((s = inet_conn(host, port, SOCK_STREAM)) < 0) {
	    (void) fprintf(stderr, "RPC Error: bad inet connect\n");
	    return RPC_ERROR;
	}
    } else {
	(void) fprintf(stderr, "RPC Error: protocol not supported: %s\n", protocol);
	return RPC_ERROR;
    }

    if ((*sendStream = fdopen(s, "w")) == NULL) {
	(void) fprintf(stderr, "RPC Error: can not fdopen for write in application\n");
	return RPC_ERROR;
    }
    
    if ((*receiveStream = fdopen(s, "r")) == NULL) {
	(void) fprintf(stderr, "RPC Error: can not fdopen for read in application\n");
	return RPC_ERROR;
    }
    
    return RPC_OK;
}

#ifndef VAXLISP
/*
 * this routine is in VAXLISP in the VAXLISP version
 */

/*
 * handle remote events FROM VEM and User Selection
 *
 * Arguments:
 *   funcArray	array of function pointers and menu strings
 *   size	number of function pointer and menu string pairs
 *
 * Returns:
 *   RPC_ERROR on error condition
 *   otherwise, never returns
 */

rpcStatus
RPCApplicationProcessEvents(funcArray, size)
RPCFunction funcArray[];
long size;			/* number of items in the array */
{
    int rmask;			/* read selection mask */
    int wmask;			/* write selection mask */
    int emask;			/* exception selection mask */
    int nfound;			/* number of items selected */
    long functionNumber;	/* user function number */
    RPCSpot spot;
    lsList argList;		/* user function arguments */
    long userOptionWord;

    /* set up the masks */
    rmask = FDMASK(fileno(RPCReceiveStream)) | RPCUserSelection.ReadMask;
    wmask = RPCUserSelection.WriteMask;
    emask = RPCUserSelection.ExceptionMask;

    /*
     * loop waiting for VEM to request user functions
     */
loop:
    while ((nfound = select(32, &rmask, &wmask, &emask,
			    RPCNIL(struct timeval))) >= 0) {

	/* timeout or error */
	if (nfound <= 0) {
	    perror("select fail");
	    return RPC_ERROR;
	}

	if (FDMASK(fileno(RPCReceiveStream)) & rmask) {
	    nfound--;

	    /* get the function number */
	    if (!RPCReceiveFunctionToken(&functionNumber, RPCReceiveStream)) {
		return RPC_ERROR;
	    }

	    /* special case demon functions */
            if (functionNumber == RPC_DEMON_FUNCTION) {
                octObject changeList;
                long id;
                void (*ptr)();                /* was char *ptr */
                void (*func)();

		/* get the octId of the change list */
		if (RPCReceiveLong(&id, RPCReceiveStream) != RPC_OK) {
		    return RPC_ERROR;
                }
		/* see if a demon has really been registered on the change list */
/* fprintf(stderr, "client here: just received a demon request\n"); */
                if (st_lookup(RPCDemonTable, (char *) id, (char**)&ptr)) {
                    ohGetById(&changeList, (octId) id);
                    func = ptr;
/* fprintf(stderr, "client here: found the corresponding demon\n"); */
		    /* run the function */
		    (*func)(&changeList);
/* fprintf(stderr, "client here: finished the demon\n"); */

		    /* clean up */
                    st_delete(RPCDemonTable, (char **)&id, (char **)&ptr);
		} 
/*
		else {
fprintf(stderr, "client here: could not find the demon\n");
		}
*/
		if (RPCApplicationFunctionComplete() != RPC_OK) {
		    return RPC_ERROR;
		}
/* fprintf(stderr, "client here: sent application complete message\n"); */
            } else if (functionNumber <= size) {

		/* user rpc functions */
		if (RPCReceiveVemArgs(&spot, &argList, &userOptionWord,
				      RPCReceiveStream) != RPC_OK) {
		    (void) fprintf(stderr,
				   "RPC Error: getting args\n");
		    return RPC_ERROR;
		}

		/* call the user function */
		if ((*funcArray[functionNumber].function)(&spot, argList,
			   userOptionWord) != RPC_OK) {
		    (void) fprintf(stderr, "RPC Error: user function\n");
		    return RPC_ERROR;
		}

		/* clean up */
		if (RPCApplicationFunctionComplete() != RPC_OK) {
		    return RPC_ERROR;
		}
	    } else {
		(void) fprintf(stderr, "RPC Error: bad function number\n");
		return RPC_ERROR;
	    }
	}

	if ((nfound > 0) &&
	    ((RPCUserSelection.ReadMask & rmask)
	     || (RPCUserSelection.WriteMask & wmask)
	     || (RPCUserSelection.ExceptionMask & emask))) {
	    (*RPCUserSelection.Function)(nfound,
					 (int *) &rmask,
					 (int *) &wmask,
					 (int *) &emask);
	}

	/* reset the masks */
	rmask = FDMASK(fileno(RPCReceiveStream)) | RPCUserSelection.ReadMask;
	wmask = RPCUserSelection.WriteMask;
	emask = RPCUserSelection.ExceptionMask;
    }

    if (errno == EINTR) {
	goto loop;
    }
    
    (void) fprintf(stderr, "RPC Error: interrupted select\n");

    return RPC_ERROR;
}

#endif /* VAXLISP */


rpcStatus
RPCApplicationFunctionComplete()
{
    long dummy;

    if (!RPCSendLong(VEM_COMPLETE_FUNCTION, RPCSendStream)) {
	(void) fprintf(stderr, "RPC Error: function end failure\n");
	return RPC_ERROR;
    }

    RPCFLUSH(RPCSendStream);

    if (!RPCReceiveLong(&dummy, RPCReceiveStream)) {
	(void) fprintf(stderr, "RPC Error: function end failure\n");
	return RPC_ERROR;
    }

    return RPC_OK;
}


#ifdef VAXLISP

/*
 * routines for the VAXLISP version
 */

long
RPCInitApplication(host, display, port, protocol,
		   byte1, byte2, byte3, byte4, integer,
		   spot, arglist, uow)
char *host;					/* server host		*/
char *display;					/* vem display		*/
int port;					/* server port		*/
char *protocol;                                /* protocol - "decnet" or "inet" */
int byte1, byte2, byte3, byte4, integer;        /* for byte swapping test */
RPCSpot *spot;  				/* initial spot		*/
lsList *arglist;				/* initial argument list */
long *uow;					/* initial user option word */
/*
 * initialize a remote application
 *
 *  - set up the connection
 *  - send the menu and other things
 */
{
    char *disp;

    (void) RPCByteSwappedApplication(byte1, byte2, byte3, byte4, integer);
    
    if (RPCConnectToServer(host, port, protocol, &RPCSendStream, &RPCReceiveStream)
	== RPC_ERROR) {
	return RPC_ERROR;
    }

    if (vemInitializeApplication(&disp, spot, arglist, uow)
	!= OCT_OK) {
	return RPC_ERROR;
    }

    return RPC_OK;
}


long
RPCSetUpMasks(rmask, wmask, emask)
int *rmask, *wmask, *emask;		/* selection masks	*/
/*
 * initialize/reset the selection masks
 */
{
    *rmask = FDMASK(fileno(RPCReceiveStream)) | RPCUserSelection.ReadMask;
    *wmask = RPCUserSelection.WriteMask;
    *emask = RPCUserSelection.ExceptionMask;
    return RPC_OK;
}


long
RPCEvent(rmask)
unsigned long rmask;		/* read selection mask		*/
/*
 * was the selection event on the RPC stream
 */
{
    if (FDMASK(fileno(RPCReceiveStream)) & rmask) {
	return RPC_OK;
    } else {
	return RPC_ERROR;
    }
}


extern int errno;

long
RPCInterrupted()
/*
 * was the select interrupted
 */
{
    if (errno == EINTR) {
	return RPC_OK;
    } else {
	(void) fprintf(stderr, "RPC Error: interrupted select\n");
	return RPC_ERROR;
    }
}


/* routine to copy C data to LISP space */

int 
RPCCtoLISPvemPointArg(array, ptr, count)
struct RPCPointArg *array;
struct RPCPointArg *ptr;
int count;
{
    (void) memcpy((char *) ptr, (char *) array, count * sizeof(struct RPCPointArg));
    return 1;
}

#undef octGenerate

octStatus
octGenerate(generator, object)
octGenerator *generator;
octObject *object;
{
    return (*(generator)->generator_func)(generator, object);
}

#endif /* VAXLISP */

