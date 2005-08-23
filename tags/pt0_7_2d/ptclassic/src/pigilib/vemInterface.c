/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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
/* vemInterface.c  edg
Version identification:
@(#)vemInterface.c	1.26 12/10/97
Functions that interface C to VEM
Note: all print functions append \n to string before printing.
*/

/* Includes */

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>

/* Octtools includes */
#include "oct.h"
#include "list.h"		/* define prototype for lsCreate */
#include "rpc.h"
#include "rpcApp.h"

/* Pigilib includes */
#include "vemInterface.h"
#include "ptk.h"
#include "err.h"
#include "octMacros.h"
#include "xfunctions.h"

#define VemReady() (RPCSendStream != 0)

/* Controls whether errors get printed out in windows */
static boolean errorWindows = TRUE;

#define REPORT_TCL_ERRORS 0

void
ViSetErrWindows(state)
boolean state;
{
    errorWindows = state;
}

boolean
ViGetErrWindows()
{
    return (errorWindows);
}


/* PrintCon  5/10/88 4/7/88
Print string to VEM console window only.
Use this instead of VEMMSG() to get VEM prompt.
*/
void
PrintCon(s)
const char *s;
{
    if (VemReady()) {
	vemMessage("\0062", MSG_DISP|MSG_NOLOG);
	vemMessage(s, MSG_DISP|MSG_NOLOG);
	vemMessage("\n\0060", MSG_DISP|MSG_NOLOG);
    }
    else printf ("%s\n", s);
}

/* PrintConLog  5/10/88 4/7/88
Print string to VEM console window and VEM log file.
*/
void
PrintConLog(s)
const char *s;
{
    if (VemReady()) {
	vemMessage("\0062", MSG_DISP|MSG_NOLOG);
	vemMessage(s, MSG_DISP);
	vemMessage("\n\0060", MSG_DISP);
    }
    else printf ("%s\n", s);
}

/* PrintErr  5/10/88 4/25/88 7/15/93 2/15/96
Print error message to VEM console window and VEM log file.
Change later to different color from PrintConLog().
*/
void
PrintErr(s)
const char *s;
{
#ifdef OLDNEVER
    char buf[MSG_BUF_MAX];
#else
    char buf[1024];
#endif /* OLDNEVER */
#if REPORT_TCL_ERRORS
    char *msg;
#endif
    if (*s == 0) return;        /* ignore blank message */
    if (!VemReady()) {
	fprintf (stderr, "Error: %s\n", s);
	return;
    }
    (void) sprintf(buf, "\0062Error: %.1000s\n\0060", s);
    (void) vemMessage(buf, MSG_DISP);
    if (errorWindows) {
	/* print error message in a dialog box */
 	(void) sprintf(buf, "Error: %.1000s", s);
	win_msg (buf);
    }
#if REPORT_TCL_ERRORS
    msg = Tcl_GetVar(ptkInterp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
        msg = ptkInterp->result;
    }
    (void) vemMessage(msg, MSG_DISP);
    win_msg (msg);
#endif
}

/* PrintErrNoTag: like PrintErr, but don't prepend "Error:".
*/
void
PrintErrNoTag(s)
const char *s;
{
#ifdef OLDNEVER
    char buf[MSG_BUF_MAX];
#else
    char buf[1024];
#endif /* OLDNEVER */
#if REPORT_TCL_ERRORS
    char *msg;
#endif
    if (*s == 0) return;        /* ignore blank message */
    if (!VemReady()) {
	fprintf (stderr, "%s\n", s);
	return;
    }
    (void) sprintf(buf, "\0062%.1000s\n\0060", s);
    (void) vemMessage(buf, MSG_DISP);
    if (errorWindows) {
	/* print error message in a dialog box */
 	(void) sprintf(buf, "%.1000s", s);
	win_msg (buf);
    }
#if REPORT_TCL_ERRORS
    msg = Tcl_GetVar(ptkInterp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
        msg = ptkInterp->result;
    }
    (void) vemMessage(msg, MSG_DISP);
    win_msg (msg);
#endif
}


/* PrintDebug  5/10/88 4/7/88 2/15/96
Print debug info if PrintDebug is turned on.
*/
static int printDebugState = TRUE;

void
PrintDebug(s)
const char *s;
{
#ifdef OLDNEVER
    char buf[MSG_BUF_MAX];
#else
    char buf[1024];
#endif /* OLDNEVER */
    if (printDebugState) {
 	(void) sprintf(buf, "\0062Debug: %.1000s\n\0060", s);
	(void) vemMessage(buf, MSG_DISP);
    }
}

void
PrintDebugSet(state)
boolean state;
{
    printDebugState = state;
}

boolean
PrintDebugGet()
{
    return(printDebugState);
}

static char *fnName;

void
ViInit(name)
char *name;
{
    fnName = name;
}
    
void
ViTerm()
{
    vemMessage("\0060", MSG_DISP|MSG_NOLOG);
    vemMessage(fnName, MSG_DISP|MSG_NOLOG);
    vemMessage(" finished \n\0060", MSG_DISP|MSG_NOLOG);
}

char *
ViGetName()
{
    return (fnName);
}
    

/* 8/25/89 8/1/89
Kill a VEM buffer associated with a facet if one exists so facet can be
overwritten or copied into.
Caveats: the mode of the facet gets modified
Taken from sparcs initRpcOct.c.
*/
boolean
ViKillBuf(facetPtr)
octObject *facetPtr;
{
    RPCSpot tmpSpot;
    octStatus status;

    facetPtr->contents.facet.mode = "a";
    if ((status = octOpenFacet(facetPtr)) == OCT_ALREADY_OPEN) {
	tmpSpot.facet = facetPtr->objectId;
	PrintCon("ViKillBuf: Please answer YES to the next question!");
	ERR_IF2(vemCommand("kill-buffer", &tmpSpot, lsCreate(), 0) != VEM_OK,
	    "ViKillBuf: unable to kill buffer");

	/* See if the kill was successful */
        if ((status = octOpenFacet(facetPtr)) == OCT_ALREADY_OPEN) {
            ErrAdd("ViKillBuf: Vem buffer not killed. Maybe open windows have instances of the old icon?\nClose all windows containing the old icons and try again.");
	    octCloseFacet(facetPtr);
            return (FALSE);
        }
    }
    if (status == OCT_OLD_FACET || status == OCT_NEW_FACET) {
	CK_OCT(octCloseFacet(facetPtr));
    } else {
	ErrAdd(octErrorString());
	return (FALSE);
    }
    return (TRUE);
}

/* We do the following to trim trailing whitespace the user might
   type in (or extra returns) which have a tendency to screw us up.
   dmMultiTextTrim is a version that trims trailing blanks, linefeeds,
   etc. from user entries.
*/

static void trim (s)
char *s;
{
	char *e;
	if (*s == 0) return;
	/* find end of string */
	e = s;
	while (*e) e++;
	/* back up, deleting spaces, tabs, newlines, control chars */
	while (e >= s && *e <= ' ') *e-- = 0;
}

vemStatus
dmMultiTextTrim(name, nItems, items)
char *name;
int nItems;
dmTextItem items[];
{
	int i;
	vemStatus result = dmMultiText(name, nItems, items);
	for (i = 0; i < nItems; i++)
		trim (items[i].value);
	return result;
}


/*
 * New functionality added to help prevent deadly-embrace errors in the
 * pigi-to-Vem protocol.
 *
 * When we are about to start issuing commands to Vem, we should call
 * VemLock().  It will lock Vem to prevent Vem from sending any more
 * asynchronous commands until VemUnlock() is called.  As a byproduct,
 * any already-pending asynchronous commands from Vem will be serviced.
 *
 * Lock/Unlock calls can be nested; it is critical that an eventual
 * unlock be done for every lock!
 *
 * This is bulletproof only to the extent that every place that sends
 * commands to Vem (and expects a response) does VemLock/VemUnlock.
 * Currently, we invoke locking in POct.cc, PVem.cc, compile.c and
 * ptkTclIfc.c; but there may be other places that should do it too.
 * CAUTION: some places should perhaps NOT lock, because of the possible
 * side effect of executing a command from Vem.
 *
 * ************ The lock protocol: ***************
 *
 * To lock Vem, we send a VEM_LOCKING_FUNCTION call with a +1 change
 * to Vem's busy status.  Vem will increment its status (thus preventing
 * it from sending any asynchronous commands) and return an
 * RPC_LOCK_RESPONSE_FUNCTION response.  We catch the response in the
 * normal RPC event handler (see ptkNet.c).  We do this because Vem might
 * just have queued an asynchronous command, so the first thing back through
 * the pipe isn't necessarily the lock response; we want to be able to
 * execute that command normally.
 *
 * When Vem has queued an async command at about the same time pigi is
 * trying to lock, the sequence of events is a little tricky:
 * 1. Vem queues async command and marks itself busy.
 * 2. VemLock() sends LOCK request.  (Vem will receive this, increment
 *    its state to 2, and queue a lock response.  But the async command
 *    is already in the pipe.)
 * 3. VemLock() calls Tk_DoOneEvent() which eventually calls ptkNet.c to
 *    read data from the RPC pipe.
 * 4. ptkNet.c extracts the async command and its arguments from the pipe,
 *    then calls VemFinishLock() *before* trying to execute the command.
 * 5. VemFinishLock() recursively calls ptkNet.c to obtain the lock
 *    response.  (If Vem could send more than one async command, we could
 *    recurse more than one level here.)
 * 6. After the lock response is finally obtained, ptkNet.c executes the
 *    async command, at completion of which Vem's state decrements to 1.
 * 7. We unwind back to VemLock(), and proceed with the pigi operation.
 * 8. Eventually, VemUnlock() is called to return Vem to state 0.
 *
 * If we did not extract the lock response from the pipe before executing
 * the async command, then any attempt to read data from Vem within the
 * async command would end up treating the lock response as data.
 *
 * The unlock sequence also sends VEM_LOCKING_FUNCTION and receives
 * RPC_LOCK_RESPONSE_FUNCTION.  However, it need not worry about
 * unexpected responses, since Vem is (presumably) locked.
 */

/* flag variables updated by ptkNet.c when lock response is received. */
extern long LockResponseReceived;
extern long LockResponseState;

/* Local count of nested VemLock() calls, to avoid unnecessary Vem traffic.
 * (Sending nested locks to Vem works, but wastes RPC round trips...)
 */
static long VemLockCount = 0;

/* flag for lock-in-progress */
static int VemLockInProgress = 0;


boolean
VemLock()
{
    long priorReceived = LockResponseReceived;
    long priorState = LockResponseState;

    /* already locked? */
    if (VemLockCount > 0) {
	VemLockCount++;
	return TRUE;
    }

    /* issue lock command (delta +1) */

    if (!RPCSendLong(VEM_LOCKING_FUNCTION, RPCSendStream)) {
	(void) fprintf(stderr, "Failed to lock Vem\n");
	return FALSE;
    }

    if (!RPCSendLong((long) 1, RPCSendStream)) {
	(void) fprintf(stderr, "Failed to lock Vem\n");
	return FALSE;
    }

    RPCFLUSH(RPCSendStream);

    VemLockInProgress++;

    /* Service "file" events on the Vem RPC input.
     * We do not fall out of the loop until the lock response comes back.
     */

    for (;;) {
      Tk_DoOneEvent(TK_FILE_EVENTS);
      if (LockResponseReceived != priorReceived) {
	/* Depending on whether this activity was originally started from
	 * Vem or from pigiRpc, Vem might be locked or not when it gets
	 * the lock command; we can't assume that priorState matches
	 * Vem's state.  So the only useful test is that we are now locked.
	 */
	if (LockResponseState <= 0) {
	  (void) fprintf(stderr, "Fishy lock response from Vem (%ld => %ld), proceeding anyway\n",
			 priorState, LockResponseState);
	}
	break;
      }
    }

    /* We set, rather than increment, VemLockCount because it might
     * already have been set to 1 by VemFinishLock.
     */
    VemLockCount = 1;

    VemLockInProgress--;
    return TRUE;
}


/*
 * This function is ONLY to be called from ptkNet.c, so it's not declared
 * in vemInterface.h.  It is called just before executing a user RPC
 * function call request received from Vem.  If we are in the middle
 * of a lock handshake, we have to wait for the lock response before
 * executing the RPC function --- otherwise, if the RPC function expects
 * to read any data from Vem, it will mistake the lock response for the
 * start of its data.
 */

void VemFinishLock()
{
    long priorReceived = LockResponseReceived;
    long priorState = LockResponseState;

    /* Nothing to do unless a lock handshake is in progress */
    if (VemLockInProgress == 0)
	return;

    /* Wait for the lock response to be received */
    for (;;) {
      Tk_DoOneEvent(TK_FILE_EVENTS);
      if (LockResponseReceived != priorReceived) {
	/* Depending on whether this activity was originally started from
	 * Vem or from pigiRpc, Vem might be locked or not when it gets
	 * the lock command; we can't assume that priorState matches
	 * Vem's state.  So the only useful test is that we are now locked.
	 */
	if (LockResponseState <= 0) {
	  (void) fprintf(stderr, "Fishy lock response from Vem (%ld => %ld), proceeding anyway\n",
			 priorState, LockResponseState);
	}
	break;
      }
    }

    /* Since Vem is now successfully locked, we can set VemLockCount
     * immediately rather than waiting until we unwind back to VemLock().
     * This will save a round trip handshake if the RPC function itself
     * contains lock/unlock requests.
     */
    VemLockCount = 1;
}


void VemUnlock()
{
    long priorReceived = LockResponseReceived;
    long priorState = LockResponseState;

    /* don't unlock if still nested */
    if (--VemLockCount > 0)
	return;

    /* issue unlock command (delta -1) */

    if (!RPCSendLong(VEM_LOCKING_FUNCTION, RPCSendStream)) {
	(void) fprintf(stderr, "Failed to unlock Vem\n");
	return;
    }

    if (!RPCSendLong((long) -1, RPCSendStream)) {
	(void) fprintf(stderr, "Failed to unlock Vem\n");
	return;
    }

    RPCFLUSH(RPCSendStream);

    /* Service "file" events on the Vem RPC input.
     * We do not fall out of the loop until the lock response comes back.
     */

    for (;;) {
      Tk_DoOneEvent(TK_FILE_EVENTS);
      if (LockResponseReceived != priorReceived) {
	/* Vem's lock level might have dropped by either 1 or 2 since
	 * VemLock(); the latter case occurs if an async command was
	 * executed as part of the lock handshake.  So rather than
	 * check for LockResponseState == priorState-1, we just expect
	 * it to have decreased.
	 */
	if (LockResponseState >= priorState) {
	  (void) fprintf(stderr, "Fishy unlock response from Vem (%ld => %ld), proceeding anyway\n",
			 priorState, LockResponseState);
	}
	break;
      }
    }
}
