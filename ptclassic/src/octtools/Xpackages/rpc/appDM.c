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
/*LINTLIBRARY*/
#include "copyright.h"
#include "port.h"

/*
 * dialog manager stubs
 *
 * Rick L Spickelmier, 7/24/88
 */

/* #include "vemDM.h"   */
#include "rpcApp.h"

/*
 * vem dialog manager stuff - only moded ones
 */

/*ARGSUSED*/
vemStatus
dmMultiWhich(title, count, items, item_func, help_buf)
char *title;
int count;
dmWhichItem *items;
int (*item_func)();
char *help_buf;
{
    int i;
    long returnCode, dummy;
    
    RPCASSERT(RPCSendLong(DM_MULTI_WHICH_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(title, RPCSendStream));
    RPCASSERT(RPCSendLong((long) count, RPCSendStream));
    
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCSendString(items[i].itemName, RPCSendStream));
    }
    
    RPCASSERT(RPCSendString(help_buf, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);
    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	items[i].flag = (int) dummy;
    }

    return (vemStatus) returnCode;
}


/*ARGSUSED*/
vemStatus
dmWhichOne(title, count, items, itemSelected, item_func, help_buf)
char *title;
int count;
dmWhichItem *items;
int *itemSelected;
int (*item_func)();
char *help_buf;
{
    int i;
    long returnCode, item, dummy;
        
    RPCASSERT(RPCSendLong(DM_WHICH_ONE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(title, RPCSendStream));
    RPCASSERT(RPCSendLong((long) count, RPCSendStream));
    
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCSendString(items[i].itemName, RPCSendStream));
    }
    
    RPCASSERT(RPCSendString(help_buf, RPCSendStream));
    
    RPCFLUSH(RPCSendStream);
    
    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));
    
    RPCASSERT(RPCReceiveLong(&item, RPCReceiveStream));

    *itemSelected = (int) item;
      
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveLong(&dummy, RPCReceiveStream));
	items[i].flag = (int) dummy;
    }
    
    return (vemStatus) returnCode;
}


vemStatus
dmMultiText(title, count, items)
char *title;
int count;
dmTextItem *items;
{
    int i;
    long returnCode;
    
    RPCASSERT(RPCSendLong(DM_MULTI_TEXT_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(title, RPCSendStream));
    RPCASSERT(RPCSendLong((long) count, RPCSendStream));
    
    for (i = 0; i < count; i++) {
	RPCASSERT(RPCSendString(items[i].itemPrompt, RPCSendStream));
	RPCASSERT(RPCSendLong((long) items[i].rows, RPCSendStream));
	RPCASSERT(RPCSendLong((long) items[i].cols, RPCSendStream));
	RPCASSERT(RPCSendString(items[i].value, RPCSendStream));
    }

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    for (i = 0; i < count; i++) {
	RPCASSERT(RPCReceiveString(&(items[i].value), RPCReceiveStream));
    }
        
    return (vemStatus) returnCode;
}


vemStatus
dmConfirm(title, message, yesButton, noButton)
char *title;
char *message;
char *yesButton, *noButton;
{
    long returnCode;
    
    RPCASSERT(RPCSendLong(DM_CONFIRM_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(title, RPCSendStream));
    RPCASSERT(RPCSendString(message, RPCSendStream));
    RPCASSERT(RPCSendString(yesButton, RPCSendStream));
    RPCASSERT(RPCSendString(noButton, RPCSendStream));

    RPCFLUSH(RPCSendStream);

    RPCASSERT(RPCReceiveLong(&returnCode, RPCReceiveStream));

    return (vemStatus) returnCode;
}


void
dmMessage(title, message, formatp)
char *title;
char *message;
int formatp;
{
    RPCASSERT(RPCSendLong(DM_MESSAGE_FUNCTION, RPCSendStream));
    RPCASSERT(RPCSendString(title, RPCSendStream));
    RPCASSERT(RPCSendString(message, RPCSendStream));
    RPCASSERT(RPCSendLong(formatp, RPCSendStream));
    RPCFLUSH(RPCSendStream);
    return;
}
