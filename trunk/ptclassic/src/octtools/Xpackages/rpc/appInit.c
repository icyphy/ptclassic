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
 * initialization routines for an RPC application
 *
 * Rick L Spickelmier, 7/24/88
 */

/* #include "message.h" */
/* #include "selset.h"  */
/* #include "region.h"  */
/* #include "vemDM.h"   */
/* #include "list.h"    */
#include "rpcApp.h"

int RPCFloatingPointSame;



/*
 * dummy function to get lint to check UserMain arguments
 */

#ifdef lint
void
RPCDummyFunction()
{
    char *display;
    RPCSpot *spot;
    lsList cmdList;
    long uow;
    RPCFunction **array;

    display = "shambhala:0";
    spot = (RPCSpot *) 0;
    cmdList = lsCreate();
    uow = 0;
    array = (RPCFunction **) 0;

    (void) UserMain(display, spot, cmdList, uow, array);

    RPCDummyFunction();	/* to get rid of 'but never used' message */

    return;
}
#endif /* lint */


/*
 * tell VEM about the application
 *
 * Returns:
 *   OCT_OK	if the initialization succeeded	
 *   OCT_ERROR	if anything failed
 *
 */
octStatus
vemInitializeApplication(display, spot, cmdList, userOptionWord)
char **display;
RPCSpot *spot;
lsList *cmdList;
long *userOptionWord;
{
    long format;
    double dbl;
    long *lngs;

    RPCDemonTable = st_init_table(st_numcmp, st_numhash);

    RPCASSERT(RPCSendLong(VEM_INITIALIZE_APPLICATION_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong(RPC_VERSION, RPCSendStream));

    /* send the process id - for possible killing */
    RPCASSERT(RPCSendLong((long) getpid(), RPCSendStream));

    /* ship across floating point number format information */
    dbl = RPCDOUBLE;
    lngs = (long *) &dbl;
    RPCASSERT(RPCSendLong((long) lngs[0], RPCSendStream));
    RPCASSERT(RPCSendLong((long) lngs[1], RPCSendStream));
    
    RPCFLUSH(RPCSendStream);

    /* get the floating point number format information */
    RPCASSERT(RPCReceiveLong(&format, RPCReceiveStream));
    if (format == OCT_OK) {
	RPCFloatingPointSame = 1;
    } else {
	RPCFloatingPointSame = 0;
    }
	 
    /* get the display */
    RPCASSERT(RPCReceiveString(display, RPCReceiveStream));
    RPCASSERT(RPCReceiveVemArgs(spot, cmdList, userOptionWord, RPCReceiveStream));
    return OCT_OK;
}


octStatus
vemSendMenu(array, count)
RPCFunction array[];
long count;
{
    long returnCode;
    int i;

    RPCASSERT(RPCSendLong(VEM_SEND_MENU_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendLong(count, RPCSendStream));

    /* send the pane and menu strings, and the keybindings */
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCSendString(array[i].paneString, RPCSendStream));
	RPCASSERT(RPCSendString(array[i].menuString, RPCSendStream));
	RPCASSERT(RPCSendString(array[i].keyBinding, RPCSendStream));
	RPCASSERT(RPCSendLong(array[i].userOptionWord, RPCSendStream));
    }
    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    return (octStatus) returnCode;
}

