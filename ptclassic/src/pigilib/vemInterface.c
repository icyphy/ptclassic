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
/* vemInterface.c  edg
Version identification:
$Id$
Functions that interface C to VEM
Note: all print functions append \n to string before printing.
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include "rpc.h"
#include "err.h"
#include "octMacros.h"
#include "list.h"

/* Controls whether errors get printed out in windows */
static boolean errorWindows = TRUE;

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
    if (*s == 0) return;	/* ignore blank message */
    (void) sprintf(buf, "\0062Error: %s\n\0060", s);
    (void) vemMessage(buf, MSG_DISP);
    if (errorWindows) {
	/* print error message in a dialog box */
	(void) sprintf(buf, "Error: %s", s);
	win_msg (buf);
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
