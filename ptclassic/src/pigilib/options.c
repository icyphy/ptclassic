/*  options.c  edg
Version identification:
$Id$
*/

/* Includes */
#include <stdio.h>
#include "local.h"
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
