/* misc.c  edg
Version identification:
$Id$
*/

/* Includes */
#include <stdio.h>
#include "local.h"
#include "rpc.h"
#include "vemInterface.h"
#include "util.h"
#include "octIfc.h"
#include "octMacros.h"
#include "err.h"
#include "main.h"

#define dmWidth 80

int 
Man(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Topic", 1, 40, "man", NULL};

    ViInit("man");
    ErrClear();
    if (dmMultiText("Man", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    if (FALSE) {
	ErrAdd("Not yet implemented");
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

int
ERFilterDesign(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    char buf[512];

    ViInit("equirriple FIR design");
    ErrClear();
    sprintf(buf, "xterm -display %s -name Equirriple_FIR_design -e optfir &",
                xDisplay);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        PrintErr(sprintf(buf, "Error invoking optfir program."));
    }
    ViDone();
}

int
WFilterDesign(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    char buf[512];

    ViInit("window FIR design");
    ErrClear();
    sprintf(buf, "xterm -display %s -name Equirriple_FIR_design -e wfir &",
                xDisplay);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        PrintErr(sprintf(buf, "Error invoking wfir program."));
    }
    ViDone();
}


int 
Profile(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Key", 1, 80, NULL, NULL};
    octObject facet, inst;
    vemStatus status;
    char *info;

    ViInit("profile");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
/* set current domain.  This will change to use the domain of the star */
    if(setCurDomainF(&facet) == NULL) {
        PrintErr("Domain error in facet.");
        ViDone();
    }

    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
	/* No inst under cursor */
	if (dmMultiText("Profile", 1, &item) != VEM_OK) {
	    PrintCon("Aborted entry");
	    ViDone();
	}
	if (!KcProfile(item.value)) {
	    PrintErr(ErrGet());
	}
	ViDone();
    } else if (IsDelay(&inst)) {
	/* delay inst is under cursor */
	PrintErr("Cursor must be over a star");
	ViDone();
    } else {
	/* assume inst is a star... */
	if (!KcProfile(AkoName(inst.contents.instance.master))) {
	    PrintErr(ErrGet());
	}
	ViDone();
    }
}

/* 6/27/89 = tries to open facet read-only first */
int 
RpcOpenFacet(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem items[] = {
	{"Name", 1, dmWidth, "untitled", NULL},
	{"View", 1, dmWidth, "schematic", NULL},
	{"Facet", 1, dmWidth, "contents", NULL}
    };
#define ITEMS_N sizeof(items) / sizeof(dmTextItem)
    octObject facet, prop;
    octStatus status;

    ViInit("open-facet");
    ErrClear();
    if (dmMultiText("open-facet", ITEMS_N, items) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    status = OpenFacet(&facet, items[0].value, items[1].value, items[2].value,
	"r");
    if (status == OCT_NO_EXIST) {
	status = OpenFacet(&facet, items[0].value, items[1].value,
	    "contents", "a");
	if (status <= 0) {
	    PrintErr(octErrorString());
	    ViDone();
	} else if (status == OCT_NEW_FACET) {
	    GetOrCreatePropStr(&facet, &prop, "TECHNOLOGY", UTechProp);
	    GetOrCreatePropStr(&facet, &prop, "VIEWTYPE", "SCHEMATIC");
	    /* If facet is schematic:contents then use schematic editstyle,
		else ask...
	    */
	    if (strcmp(items[2].value, "contents") == 0 &&
		strcmp(items[1].value, "schematic") == 0) {
		GetOrCreatePropStr(&facet, &prop, "EDITSTYLE", "SCHEMATIC");
	    }
	}
    } else if (status <= 0) {
	PrintErr(octErrorString());
	ViDone();
    }
    vemOpenWindow(&facet, NULL);
    ViDone();
#undef ITEMS_N
}

int 
RpcLoadStars(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Directory", 1, 80, "", NULL};

    ViInit("load-stars");
    ErrClear();
    if (dmMultiText("load-stars", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    if (FALSE) {
	ErrAdd("Not yet implemented");
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

int 
RpcLoad(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"File", 1, 80, "", NULL};

    ViInit("load");
    ErrClear();
    if (dmMultiText("load", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    if (!KcLoad(item.value)) {
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}
