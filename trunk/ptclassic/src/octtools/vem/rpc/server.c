#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
#include "copyright.h"
#include "port.h"

/*
 * RPC server (non oct and vem specific parts)
 *
 * Rick L Spickelmier, 3/20/86, last modified 3/2/87
 *
 */

#ifdef USE_GETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <X11/Xlib.h>
#include "xvals.h"
#include "general.h"
#include "message.h"
#include "list.h"
#include "attributes.h"
#include "rpcServer.h"
#include "oh.h"
#include "serverVem.h"		/* Pick up RPCDemonFunction() */
#include "vemRPC.h"		/* Pick up vemCloseApplication() */
#include "vemMain.h"		/* Pick up vemPending() */

struct RPCApplication *RPCApplication;

rpc_fd_set *RPCSelectAccept;	/* mask for selecting on client connects */
rpc_fd_set *RPCSelectRequest;	/* mask for selecting on client requests */
rpc_fd_set *RPCFileRequest;     /* mask for selecting on file requests */

int *RPCFileDescriptors;        /* file descriptors for RPCFileRequest */

int Xfd;			/* X file descriptor */

lsList RPCDemonList;

void
rpcInit()
{
#ifdef USE_GETRLIMIT
    struct rlimit	rlp;
#endif
    /*
     * allow selection on the X file descriptor
     */
    Xfd = XConnectionNumber(xv_disp());  

    /* set up the bit arrays */
    RPCSelectRequest = RPCALLOC(rpc_fd_set);
    RPCSelectAccept = RPCALLOC(rpc_fd_set);
    RPCFileRequest = RPCALLOC(rpc_fd_set);
    RPC_FD_ZERO(RPCSelectRequest);
    RPC_FD_ZERO(RPCSelectAccept);
    RPC_FD_ZERO(RPCFileRequest);
#ifdef USE_GETRLIMIT
    getrlimit(RLIMIT_NOFILE, &rlp);
    RPCFileDescriptors = RPCARRAYALLOC(int, rlp.rlim_cur - 1);
#else
    RPCFileDescriptors = RPCARRAYALLOC(int, getdtablesize());
#endif
    RPC_FD_SET(Xfd, RPCSelectRequest);
    RPCLargestFD = MAX(RPCLargestFD, Xfd);

    RPCDemonList = lsCreate();
}

/*
 * wait for and service requests
 *
 * Returns:
 *  RPC_OK - clean exit
 *  RPC_ERROR - error
 */
rpcStatus
vemServer()
{
    int eventType;		/* type the select event */
    int application;		/* application requesting service */
    int socket;                 /* socket that was selected on */
    long functionNumber;
    STREAM receiveStream;

    /*
     * wait for remote events and process them
     *
     * RPCGetEvent handles 'accept'
     *
     * returns:
     *	    server dies cleanly (RPC_EXIT)
     *	    something bad happened, abort (RPC_ABORT)
     *	    X event (RPC_X_EVENT)
     *      file event (RPC_FILE_EVENT)
     *	    rpc event (RPC_EVENT)
     */

    /* do initial X updates */
    vemProcessXEvent();

    while ((eventType = RPCGetEvent(RPCSelectAccept, RPCSelectRequest, RPCFileRequest,
				    &application, &socket)) != RPC_EXIT) {

	/* error */
	if (eventType == RPC_ABORT) {
	    vemMessage("RPC Error: server: panic!!!  server closing\n",
			      MSG_DISP);
	    RPC_FD_ZERO(RPCSelectRequest);
	    RPC_FD_SET(Xfd, RPCSelectRequest);
	    RPC_FD_ZERO(RPCSelectAccept);
	    RPC_FD_ZERO(RPCFileRequest);
	    continue;
	}

        /* process change lists demons */
        if (lsLength(RPCDemonList) > 0) {
            Pointer ptr;
            lsGen gen = lsStart(RPCDemonList);
            while (lsNext(gen, &ptr, 0) == LS_OK) {
		struct rpcDemon *demon;
                octObject changeList, cr;

		demon = (struct rpcDemon *) ptr;
                if (ohGetById(&changeList, demon->clId) == OCT_OK) {
		    if (octGenFirstContent(&changeList, OCT_ALL_MASK, &cr) == OCT_OK) {
			if (RPCDemonFunction(demon->app, &changeList) == RPC_OK) {
			    lsDelBefore(gen, 0);
			    VEMFREE(ptr);
			}
		    }
                }
            }
	}

	/* no event */
	if (eventType == RPC_NO_EVENT) {
	    continue;
	}

	/* X event */
	if (eventType == RPC_X_EVENT) {
	    /* process X events till there are no more */
	    while (vemPending() > 0) {
		if (vemProcessXEvent() != 0) {
		    return(RPC_OK);
		}
	    }
	    continue;
	}

	/* remote file request */

	if (eventType == RPC_FILE_EVENT) {
	    /* handle a request to write to a file */
	    RPCProcessFileWriteRequest(socket, RPCFileDescriptors[socket]);
	    continue;
	}

	/* remote request received, process it */

	if (eventType == RPC_EVENT) {

	    /* RPC remote function request */
	    receiveStream = RPCApplication[application].ReceiveStream;
	    if (!RPCReceiveLong(&functionNumber, receiveStream)) {
		
		/* error usually due to the application closing connection */
		vemMessage("RPC Error: server: application exited without calling RPCExit\n", MSG_DISP);
		
		/* tell the USER that the application has closed */
		(void) sprintf(RPCErrorBuffer, "Closing Application %s on host %s\n",
				        RPCApplication[application].name,
				        RPCApplication[application].host);

		vemMessage(RPCErrorBuffer, MSG_DISP);
		
		/* tell RPC that the application has closed */
		RPCServerEnd(application);
		
		/*
		 * tell VEM that the application has closed
		 */
		(void) vemCloseApplication(RPCApplication[application].host,
					   RPCApplication[application].name,
					   application);
		continue;
	    }

	    if (functionNumber > VEM_MAX_FUNCTION) {
		if (RPCOCTRequest(application, functionNumber) != RPC_OK) {
		    vemMessage("RPC Error: server: bad OCT request\n", MSG_DISP);
		    
		    /* tell the USER that the application has closed */
		    (void) sprintf(RPCErrorBuffer, "Closing Application %s on host %s\n",
				            RPCApplication[application].name,
				            RPCApplication[application].host);
		    vemMessage(RPCErrorBuffer, MSG_DISP);
		    
		    /* tell RPC that the application has closed */
		    RPCServerEnd(application);
		    
		    /*
		     * tell VEM that the application has closed
		     */
		    (void) vemCloseApplication(RPCApplication[application].host,
					       RPCApplication[application].name,
					       application);
		    continue;
		}
	    } else {
		if (RPCVEMRequest(application, functionNumber) != RPC_OK) {
		    vemMessage("RPC Error: server: bad VEM request\n", MSG_DISP);

		    /* tell the USER that the application has closed */
		    (void) sprintf(RPCErrorBuffer, "Closing Application %s on host %s\n",
				            RPCApplication[application].name,
				            RPCApplication[application].host);
		    vemMessage(RPCErrorBuffer, MSG_DISP);

		    /* tell RPC that the application has closed */
		    RPCServerEnd(application);

		    /*
		     * tell VEM that the application has closed
		     */
		    (void) vemCloseApplication(RPCApplication[application].host,
					       RPCApplication[application].name,
					       application);
		    continue;
		}
	    }
	    
	    while (vemPending() > 0) {
		if (vemProcessXEvent() != 0) {
		    return(RPC_OK);
		}
	    }
	    continue;
	}

	/* error */
	(void) sprintf(RPCErrorBuffer, "RPC Error: server: bad event type (%d)\n",
				eventType);
	vemMessage(RPCErrorBuffer, MSG_DISP);
	/* panic!!!  Go to just X events */
	RPC_FD_ZERO(RPCSelectRequest);
	RPC_FD_SET(Xfd, RPCSelectRequest);
	RPC_FD_ZERO(RPCSelectAccept);
	RPC_FD_ZERO(RPCFileRequest);
    }

    return(RPC_OK);
}
