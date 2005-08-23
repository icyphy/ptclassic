/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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
/*  options.c  edg
Version identification:
@(#)options.c	1.14	11/9/95
*/

/* Includes */

/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */
#include <stdio.h>

/* Octtools includes */
#include "oct.h"
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define remote procedure calls */

/* Pigilib includes */
#include "options.h"
#include "vemInterface.h"
#include "edit.h"
#include "exec.h"
#include "util.h"
#include "icon.h"
#include "misc.h"
#include "err.h"
#include "kernelCalls.h"

int 
Options(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem items[] = {
	{"Debug msgs", 1, 10, NULL, NULL},
	{"Error Windows", 1, 10, NULL, NULL},
	{"Tk Event Loop", 1, 10, NULL, NULL}
    };

#define ITEMS_N sizeof(items) / sizeof(dmTextItem)
    ViInit("options");
    ErrClear();
    items[0].value = BooleanToYesNo(PrintDebugGet());
    items[1].value = BooleanToYesNo(ViGetErrWindows());
    items[2].value = BooleanToYesNo(KcEventLoopActive());
    if (dmMultiText("Options", ITEMS_N, items) != VEM_OK) {
	PrintCon("Aborted entry");
    }
    else {
	PrintDebugSet(YesNoToBoolean(items[0].value));
	ViSetErrWindows(YesNoToBoolean(items[1].value));
	KcSetEventLoop(YesNoToBoolean(items[2].value));
    }
    ViDone();
#undef ITEMS_N

}

boolean
OptInit()
{
    char *def, buf[512];
    
    sprintf(buf, "%s.DebugMsgs", UAppName);
    if ((def = RPCXGetDefault("vem", buf)) == NULL) {
	def = "on";
    }
    PrintDebugSet(YesNoToBoolean(def));

    sprintf(buf, "%s.ErrWindows", UAppName);
    if ((def = RPCXGetDefault("vem", buf)) == NULL) {
	def = "on";
    }
    ViSetErrWindows(YesNoToBoolean(def));

    return (TRUE);
}
