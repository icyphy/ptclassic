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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"
#include "appNet.h"  
/*
 * RPC user routines for the application
 *
 * Rick L Spickelmier, 7/31/86, last modified 3/2/87
 */

#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H		/* mips Ultrix4.3A requires this
				   otherwise we get conflicts with compat.h */
#include <sys/socket.h>
#endif

#include <sys/ioctl.h>
#include <sys/signal.h>

/* #include "list.h"    */
/* #include "message.h" */
#include "rpcApp.h"

struct RPCUser RPCUserSelection;	/* user selection structure */
char *RPCHostName;                      /* host name */
char *RPCProtocol;                      /* protocol name */


/*ARGSUSED*/
void
RPCUserIO(length, readmask, writemask, exceptionmask, function)
int length;			/* length of the bit arrays - ignored	*/
int *readmask, *writemask, *exceptionmask;	/* masks		*/
void (*function)();		/* function to call			*/
/*
 * declare a set of file descriptors to select on and a function
 * to call if an event occurs
 */
{
    /* first, mask off the RPC selection bit */
    *readmask &= ~FDMASK(fileno(RPCReceiveStream));

    RPCUserSelection.ReadMask = *readmask;
    RPCUserSelection.WriteMask = *writemask;
    RPCUserSelection.ExceptionMask = *exceptionmask;
    RPCUserSelection.Function = function;

    return;
}


#ifdef FIOASYNC
void
RPCsigio(fd, func)
int fd;				/* file descriptor to be interrupted on */
SIGNAL_FN (*func)();		/* function to call			*/
/*
 * this causes the function 'func' to be called whenever there is data 
 * to be read on the file descriptor 'fd'.  The reason I use the ioctl's
 * rather than the direct call (fcntl) is because DEC screwed up - 4.3 works!
 */
{
    int on = 1, pid;

    pid = getpid();

    (void) ioctl(fd, FIOASYNC, (char *) &on);
    (void) ioctl(fd, SIOCSPGRP, (char *) &pid);
    (void) signal(SIGIO, func);

    return;
}
#endif /* FIOASYNC */


/*
 * create a stream that will be sent across the link to the server
 */

extern char *RPCHost;

STREAM
RPCfopen(file, mode)
char *file;
char *mode;
{
    int socket;
    long port;
    

    /* XXX first pass, just writing to a remote file */
    if ((mode[0] != 'w') && (mode[0] != 'a')) {
	return NULL;
    }

    RPCASSERT(RPCSendLong(RPC_FILE_OPEN_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(file, RPCSendStream));
    RPCASSERT(RPCSendString(mode, RPCSendStream));
    RPCFLUSH(RPCSendStream);

    /* wait for a port to connect to */
    RPCASSERT(RPCReceiveLong(&port, RPCReceiveStream));

    if (strcmp(RPCProtocol, "inet") == 0) {
	if ((socket = inet_conn(RPCHostName, (int) port, SOCK_STREAM)) < 0) {
	    return NULL;
	}
    } else {
	(void) fprintf(stderr, "RPC Error: only inet support for RPCfopen\n");
	return NULL;
    }
    
    return fdopen(socket, mode);
}


char *
RPCVersionString()
{
    static char dummy[1024];

    (void) sprintf(dummy, "RPC Version %ld", RPC_VERSION);

    return dummy;
}



int
RPCDoExit(status)
int status;
{
    extern main();

    RPCASSERT(RPCSendLong(RPC_CLEAN_EXIT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong((long) status, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    exit(status);

    /*NOTREACHED*/
    return ((int) main);	/* hack to get main loaded !! */
}


void
RPCExit(status)
int status;
{
    (void) RPCDoExit(status);
    return;
}


char *
RPCXGetDefault(program, name)
char *program;
char *name;
{
    char *result;
    
    RPCASSERT(RPCSendLong(RPC_X_GET_DEFAULT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(program, RPCSendStream));
    RPCASSERT(RPCSendString(name, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    RPCASSERT(RPCReceiveString(&result, RPCReceiveStream));
    return result;
}


void
RPCExitVem()
{
    RPCASSERT(RPCSendLong(RPC_EXIT_VEM_FUNCTION, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    exit(0);
}

st_table *RPCDemonTable;

void
RPCRegisterDemon(changeList, func)
octObject *changeList;
void (*func)();
{
    st_insert(RPCDemonTable, (char *) changeList->objectId, (char *) func);

    RPCASSERT(RPCSendLong(RPC_REGISTER_DEMON_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong(changeList->objectId, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}

