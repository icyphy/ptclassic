/* Version Identification:
 * $Id$
 */
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

#ifndef RPCSERVER_H
#define RPCSERVER_H

/*
 * Remote Procedure Call Package for VEM (RPC)
 *
 * Copyright (c) 1986, 1987, 1988, Rick L Spickelmier.  All rights reserved.
 *
 * Use and copying of this software and preparation of derivative works
 * based upon this software are permitted.  However, any distribution of
 * this software or derivative works must include the above copyright
 * notice.
 *
 * This software is made available AS IS, and neither the Electronics
 * Research Laboratory or the University of California make any
 * warranty about the software, its performance or its conformity to
 * any specification.
 *
 * Suggestions, comments, or improvements are welcome and should be
 * addressed to:
 *
 *   Rick L Spickelmier
 *   Electronics Research Laboratory
 *   Cory Hall
 *   University of California
 *   Berkeley, CA   94720
 *
 *   rpc@eros.Berkeley.EDU  (ARPANET)
 *   ..!ucbvax!eros!rpc        (UUCP)
 *
 */

/*
 * internal header file for the RPC server
 *
 * not to be used by mere mortals (i.e. users)
 *
 * Rick L Spickelmier, 11/12/86
 *
 */

#define RPCLOCATION "server"

#include "oct.h"

#ifndef XId
#include <X11/Xlib.h>
#endif /* XId */

#include "rpcInternal.h"

/*
 * global select accept and request masks
 *
 * bits set in RPCSelectAccept signal that the server is 
 * allowed to accept connection requests on the corresponding
 * socket
 *
 * bits set in the RPCSelectRequest signal that the server is
 * allowed to read requests on the corresponding file descriptor
 *
 * bits set in the RPCFileRequest signal that the server is
 * allowed to read file requests on the corresponding file descriptor
 */

#include "bits.h"

extern rpc_fd_set *RPCSelectAccept;
extern rpc_fd_set *RPCSelectRequest;
extern rpc_fd_set *RPCFileRequest;

extern int *RPCFileDescriptors;

/*
 * global X file descriptor
 */
extern int Xfd;

extern int RPCLargestFD;

extern lsList RPCDemonList;

struct rpcDemon {
    int app;
    octId clId;
};

/*
 * remote application information
 *
 * this information is on the server side
 *   thus no user function pointer is stored
 */
struct RPCApplication {
    int active;                 /* 1 if the entry is okay, 0 is garbage */
    char *name;			/* name (path) of the application	*/
    char *host;			/* host					*/
    char *user;                 /* remote user, can be NULL             */
    char *protocol;		/* protocol - "decnet", or "inet"       */
    char *localhost;		/* local (server) host                  */
    char *display;		/* display                              */
    long startingTime;          /* unix time                            */
    Window theWin;		/* window which the command was invoked	*/
    long menuSize;		/* number of menu items			*/
    char **paneString;		/* pane array				*/
    char **menuString;		/* menu array				*/
    char **keyBinding;		/* key binding array			*/
    long *userOptionWords;	/* user option words                    */
    int  Port;			/* port for remote application to connect to */
    int  WaitSocket;		/* socket to wait form remote connection on */
    STREAM SendStream;		/* stream to send data on		*/
    STREAM ReceiveStream;	/* stream to read data on		*/
    int state;			/* 0 for nothing running, 1 for running */
    RPCSpot *spot;	        /* initial spot				*/
    lsList argList;		/* initial argument list		*/
    long userOptionWord;	/* initial user option word		*/
    lsList clonedBags;		/* list of cloned bags			*/
    long pid;			/* process id of the remote applcation	*/
};


/*
 * remote application array, maximum number of applications (size of array)
 */
extern struct RPCApplication *RPCApplication;
extern int RPCMaxApplications;

/* 
 * RPCGetEvent Codes
 *  produced by RPCGetEvent, user by the server event handler loop
 */
#define RPC_EXIT	-2
#define RPC_ABORT	-1
#define RPC_NO_EVENT	 0
#define RPC_X_EVENT	 1
#define RPC_EVENT	 2
#define RPC_FILE_EVENT	 3


extern void		rpcInit(); /* Called in main/vem.c */

/* handle connection/network operations */
extern rpcStatus	RPCAddApplication();
extern void		RPCServerEnd();

extern int		RPCGetEvent();
extern rpcStatus	RPCSetUpSocket();

extern rpcStatus	vemServer();

/* user functions */
extern rpcStatus	RPCUserFunction();	/* called by VEM menu stuff */

/* handle VEM requests */
extern rpcStatus	RPCVEMRequest();
extern rpcStatus	RPCVemMessage();
extern rpcStatus	RPCVemArgs();
extern rpcStatus	RPCVemInitialize();

/* handle OCT requests */

/* server */
extern rpcStatus	RPCOCTRequest();
/* extern rpcStatus	RPCServerOctGenObjects(); */
/* extern rpcStatus	RPCServerOctOneObject();  */
/* extern rpcStatus	RPCServerOctTwoObjects(); */
extern rpcStatus	RPCOctBB();
extern rpcStatus	RPCOctGenerate();
/* extern rpcStatus	RPCOctPoints();           */

#endif /* RPCSERVER_H */
