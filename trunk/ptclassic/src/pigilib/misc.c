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
#include "list.h"

#define dmWidth 40

int 
Man(spot, cmdList, userOptionWord) /* ARGSUSED */
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
PrintFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Options (e.g. -p -b 3x3):", 1, 40, "-x -X", NULL};
    char buf[512];
    octObject facet;
    char* fullName;

    ViInit("print facet");
    ErrClear();

    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    octFullName(&facet,&fullName);

    if (dmMultiText("print facet (PRINTER variable must be set)", 1, &item) != VEM_OK) {
        PrintCon("Aborted entry");
        ViDone();
    }
    sprintf(buf, "prfacet %s %s", item.value, fullName);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        sprintf(buf, "Error invoking prfacet utility.");
	PrintErr(buf);
    }
    ViDone();
}


int
ERFilterDesign(spot, cmdList, userOptionWord) /* ARGSUSED */
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
        sprintf(buf, "Error invoking optfir program.");
	PrintErr(buf);
    }
    ViDone();
}

int
WFilterDesign(spot, cmdList, userOptionWord) /* ARGSUSED */
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
        PrintErr("Error invoking wfir program.");
    }
    ViDone();
}


int 
Profile(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    static dmTextItem item = {"Star or Target name", 1, 40, NULL, NULL};
    octObject facet, inst;
    vemStatus status;

    ViInit("profile");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
    } else if (status != VEM_OK) {
	/* No inst under cursor */
	if (dmMultiText("Profile", 1, &item) != VEM_OK) {
	    PrintCon("Aborted entry");
	}
	else if (!KcProfile(item.value)) {
	    PrintErr(ErrGet());
	}
    } else if (IsStar(&inst)) {
	if(setCurDomainInst(&inst) == NULL) {
	    PrintErr("Domain error in instance.");
	}
	    /* autoload the star if necessary */
	else if (!AutoLoadCk(&inst) ||
	    !KcProfile(AkoName(inst.contents.instance.master))) {
	    PrintErr(ErrGet());
	}
    } else if (IsGal(&inst)) {
	if (!CompileGalInst(&inst,&facet) ||
	    !KcProfile(AkoName(inst.contents.instance.master))) {
	    PrintErr(ErrGet());
	}
    } else {
	/* delay or palette inst, or universe is under cursor */
	PrintErr("Cursor must be over a star or galaxy instance");
    }
    ViDone();
}

/* 6/27/89 = tries to open facet read-only first */
int 
RpcOpenFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
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
    RPCArg *theArg;
    Window newWindow;  /* the vem window looking at facet */
    octBox bbox;  /*  an argument to octBB, return value not used */
    void AdjustScalePan(); /* this function is right after RpcOpenFacet */

    ViInit("open-facet");
    ErrClear();

    /* see if the facet name was given as a Vem argument */
    if ((lsLastItem(cmdList, (lsGeneric *) &theArg, LS_NH) == VEM_OK) &&
	(theArg->argType == VEM_TEXT_ARG)) {
	/* set default values */
	facet.contents.facet.view = "schematic";
	facet.contents.facet.facet = "contents";
	if (ohUnpackFacetName(&facet, theArg->argData.string) != OCT_OK) {
	    PrintErr("Cannot parse window specification");
	    ViDone();
	}
	else {
	    char buf[512];
	    items[0].value = facet.contents.facet.cell;
	    items[1].value = facet.contents.facet.view;
	    items[2].value = facet.contents.facet.facet;
	    sprintf (buf, "opening %s:%s:%s", items[0].value, items[1].value,
		     items[2].value);
	    PrintDebug (buf);
	    sleep(1);
	}
    }
    /* not on command line, prompt for it */
    else if (dmMultiText("open-facet", ITEMS_N, items) != VEM_OK) {
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
    newWindow = vemOpenWindow(&facet, NULL);
    /* if the facet has no bounding box, that means it has no geometry
       to define a bounding box, then we zoom-out to the appropriate scale
       and pan to let the X-Y axes disappearing from the window. */
    if (octBB(&facet, &bbox) == OCT_NO_BB) {
        AdjustScalePan(newWindow, facet.objectId);
    }
    ViDone();
#undef ITEMS_N
}

/* this function is only used in RpcOpenFacet.  Since vem opens a window 
on a facet having no geometry with a very inconvenient scale and annoying 
cross-hair(X-Y axes), we do an automatic zoom-out  to an appropriate scale
and pan to a point far from the origin so that the cross hair won't be seen
in the window.  */
static void
AdjustScalePan(window, facetId)
Window window;
octId facetId;
{
    RPCSpot spot;
    lsList argList;
    RPCArg boxArg;
    RPCPointArg bbPoints[2];
    char *command;

    spot.theWin = window;
    spot.facet = facetId;
    /* center point of window after panning, should be far from origin. */
    spot.thePoint.x = 1000;
    spot.thePoint.y = 1000;
    argList= lsCreate();
    boxArg.argType = VEM_BOX_ARG;
    boxArg.length = 1;
    bbPoints[0].theWin = window;  bbPoints[1].theWin = window;
    bbPoints[0].facet = facetId;  bbPoints[1].facet = facetId;
    /* zoom-out to box (-1,-1)(1,1).  Since vem opens a window on a empty facet
       with corner points at (-22,22)(22,22),  the zoom-out factor is about
       22 times and is appropriate for constructing ptolemy schematic 
       diagrams.  */
    bbPoints[0].thePoint.x = -1;  bbPoints[1].thePoint.x = 1;
    bbPoints[0].thePoint.y = -1;  bbPoints[1].thePoint.y = 1;
    boxArg.argData.points = bbPoints;
    lsNewEnd(argList, (lsGeneric) &boxArg, LS_NH);
    command = "zoom-out";
    vemCommand(command, &spot, argList, (long) 0);
    command = "pan";
    vemCommand(command, &spot, argList, (long) 0);
    lsDestroy(argList, NULL);
    return;
}

/* Load the star pointed to */
static int 
DoRpcLoadStar(spot, cmdList, permB) /* ARGSUSED */
RPCSpot	*spot;
lsList	cmdList;
int	permB;
{
    int		linkCnt = 0;
    char	linkArgs[1024];
    octObject	inst;
    vemStatus	status;
    RPCArg	*theArg;

    ViInit("load-star");

    /* see if any arguments were given */
    linkArgs[0] = '\0';
    while ( lsDelBegin(cmdList, (lsGeneric *) &theArg) == VEM_OK ) {
	switch ( theArg->argType ) {
	case VEM_TEXT_ARG:
	    strcpy( linkArgs, theArg->argData.string);
	    strcat( linkArgs, " ");
	    ++linkCnt;
	    break;
	case VEM_OBJ_ARG:
	    PrintErr("Instance args not yet implemented");
	    ViDone();
	    break;
	default:
	    PrintErr("Only strings and instances allowed as link arguments");
	    ViDone();
	}
    }

    /* set the current domain */
    if (!setCurDomainS(spot)) {
        PrintErr("Invalid domain found");
	ViDone();
    }
    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
	ViDone();
    } else if (status != VEM_OK) {
	PrintErr("Cursor must be over an icon instance");
	ViDone();
    } else if (!IsStar(&inst)) {
	PrintErr("Instance is not a star");
	ViDone();
    } else {
	char *akoName = AkoName(inst.contents.instance.master);
	if (KcIsCompiledInStar(akoName)) {
	    PrintErr("Cannot re-load a compiled-in star class");
	    ViDone();
        }
	if (!LoadTheStar(&inst, permB, linkArgs))
		PrintErr(ErrGet());
	ViDone();
    }
}


/* Load the star pointed to */
int 
RpcLoadStar(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    return DoRpcLoadStar(spot, cmdList, FALSE);
}

int 
RpcLoadStarPerm(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    return DoRpcLoadStar(spot, cmdList, TRUE);
}
