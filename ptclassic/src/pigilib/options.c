/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/*  options.c  edg
Version identification:
$Id$
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include "rpc.h"
#include "vemInterface.h"
#include "edit.h"
#include "exec.h"
#include "util.h"
#include "icon.h"
#include "misc.h"
#include "err.h"

void KcSetAnimationState();

int 
Options(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem items[] = {
	{"Animation", 1, 10, NULL, NULL},
	{"Debug msgs", 1, 10, NULL, NULL},
	{"Error Windows", 1, 10, NULL, NULL}
    };
#define ITEMS_N sizeof(items) / sizeof(dmTextItem)

    ViInit("options");
    ErrClear();
    items[0].value = BooleanToYesNo(KcGetAnimationState());
    items[1].value = BooleanToYesNo(PrintDebugGet());
    items[2].value = BooleanToYesNo(ViGetErrWindows());
    if (dmMultiText("Options", ITEMS_N, items) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    KcSetAnimationState(YesNoToBoolean(items[0].value));
    PrintDebugSet(YesNoToBoolean(items[1].value));
    ViSetErrWindows(YesNoToBoolean(items[2].value));
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
