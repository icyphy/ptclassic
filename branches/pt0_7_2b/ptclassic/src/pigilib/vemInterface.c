/* vemInterface.c  edg
Version identification:
$Id$
Functions that interface C to VEM
Note: all print functions append \n to string before printing.
*/

/* Includes */
#include <stdio.h>
#include "local.h"
#include "rpc.h"
#include "err.h"
#include "octMacros.h"
#include "list.h"

/* Controls whether errors get printed out in windows */
static boolean gabWind = TRUE;

void
ViSetErrWindows(state)
boolean state;
{
    gabWind = state;
}

boolean
ViGetErrWindows()
{
    return (gabWind);
}


/* PrintCon  5/10/88 4/7/88
Print string to VEM console window only.
Use this instead of VEMMSG() to get VEM prompt.
*/
void
PrintCon(s)
char *s;
{
    vemMessage("\0062", MSG_DISP|MSG_NOLOG);
    vemMessage(s, MSG_DISP|MSG_NOLOG);
    vemMessage("\n\0060", MSG_DISP|MSG_NOLOG);
}

/* PrintConLog  5/10/88 4/7/88
Print string to VEM console window and VEM log file.
*/
void
PrintConLog(s)
char *s;
{
    vemMessage("\0062", MSG_DISP|MSG_NOLOG);
    vemMessage(s, MSG_DISP);
    vemMessage("\n\0060", MSG_DISP);
}


/* PrintErr  5/10/88 4/25/88
Print error message to VEM console window and VEM log file.
Change later to different color from PrintConLog().
*/
void
PrintErr(s)
char *s;
{
    char buf[MSG_BUF_MAX];

    (void) sprintf(buf, "\0062Error: %s\n\0060", s);
    (void) vemMessage(buf, MSG_DISP);
    if (gabWind) {
	/* print error message in a dialog box, not yet implemented */
    }
}


/* PrintDebug  5/10/88 4/7/88
Print debug info if PrintDebug is turned on.
*/
static int printDebugState = TRUE;
void
PrintDebug(s)
char *s;
{
    char buf[MSG_BUF_MAX];

    if (printDebugState) {
	(void) sprintf(buf, "\0062Debug: %s\n\0060", s);
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
    struct RPCSpot tmpSpot;
    octStatus status;

    facetPtr->contents.facet.mode = "a";
    while ((status = octOpenFacet(facetPtr)) == OCT_ALREADY_OPEN) {
	tmpSpot.facet = facetPtr->objectId;
	PrintCon("ViKillBuf: Please answer YES to the next question!");
	ERR_IF2(vemCommand("kill-buffer", &tmpSpot, lsCreate(), 0) != VEM_OK,
	    "ViKillBuf: unable to kill buffer");
    }
    if (status == OCT_OLD_FACET || status == OCT_NEW_FACET) {
	CK_OCT(octCloseFacet(facetPtr));
    } else {
	ErrAdd(octErrorString());
	return (FALSE);
    }
    return (TRUE);
}