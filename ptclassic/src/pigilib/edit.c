/* 
Copyright (c) 1990-1994 The Regents of the University of California.
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
/* edit.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <string.h>
#include <sys/file.h>
#include "local.h"
#include "rpc.h"
#include "oh.h"
#include "paramStructs.h"
#include "vemInterface.h"
#include "util.h"
#include "err.h"
#include "octIfc.h"
#include "exec.h"

#include "pigidefine.h"    /* defs of Constants */
#include "ptk.h"  /* Interpreter name, window name, etc.  aok */
#include "handle.h"

/* Declare the functions in kernelCalls for editTarget */
int KcDomainTargets();
char* KcDefTarget();


void FindClear();

#define dmWidth 40
#define dmIncrement 20

/* body of parameter-edit code.
Returns 0 if there is an error.
Returns 1 if the parameters were changed or if the user clicked OK.
Returns 2 if the command was aborted by the user.
*/

static boolean
EditParamList(pListPtr, dmTitle)
ParamListType *pListPtr;
char* dmTitle;
{
    ParamType *place;
    dmTextItem *items;
    int i, width, maxwidth;

    items = (dmTextItem *) calloc(pListPtr->length, sizeof(dmTextItem));
    place = pListPtr->array;
    width = 0;
    maxwidth = 0;
    for (i = 0; i < pListPtr->length; i++) {
	items[i].itemPrompt = place->name;
	items[i].rows = 1;
	items[i].value = place->value;
	width = strlen(place->value);
	if(maxwidth < width) maxwidth = width;
	items[i].userData = NULL;
	place++;
    }
    for (i = 0; i < pListPtr->length; i++) {
	items[i].cols = maxwidth + dmIncrement;
    }
    if (dmMultiText(dmTitle, pListPtr->length, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return 2;
    }
    /* should free place->value string before assignment */
    place = pListPtr->array;
    for (i = 0; i < pListPtr->length; i++) {
	place->value = items[i].value;
	place++;
    }
    free((char *)items);
    return(TRUE);
}

/* EditFormalParameters is still needed for the RunUniverse command.  -aok */
/* 4/14/89 3/19/89
Edit the formal parameters of a galaxy schematic facet.
Returns 0 if there is an error.
Returns 1 if the parameters were changed or if the user clicked OK.
Returns 2 if the command was aborted by the user.
*/
int
EditFormalParams(galFacetPtr)
octObject *galFacetPtr;
{
    int i;
    ParamListType pList;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));

    if (pList.length == 0) {
        PrintErr("Galaxy or Universe has no parameters");
        return(1);
    }
    i = EditParamList(&pList, "Edit Formal Parameters");
    if (i != 1) return i;
    ERR_IF1(!SetFormalParams(galFacetPtr, &pList));
    return(1);
}

/* EditParams  5/27/88 4/24/88
Edit parameters.
Changed 9/93 to use the POct code and the TCL/Tk tools - aok
*/
int 
EditParams(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet, inst;
    vemStatus status;
    char facetHandle[16], instanceHandle[16];

    ViInit("edit-params");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
    ptkOctObj2Handle(&facet,facetHandle);

    /* get name of instance under cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
	/* cursor not over an instance... */
	strcpy (instanceHandle, "NIL");
    } else {
	/* cursor is over some type of instance... */
	ptkOctObj2Handle(&inst,instanceHandle);
    }

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkEditParams ", facetHandle, " ", instanceHandle, (char *)NULL) )

    ViDone();
}

/***** Begin of RpcOpenPalette routines */

/* Structures to store palette list and dialog box */
static char **palettes;
static int palettes_n;

/* function to determine default palettes */
char *defaultPalettes();

static dmWhichItem *items;

/* ListLength  4/28/88
Counts number of ':' in string, adds one, and returns this value.
*/
static int
ListLength(pathStr)
char *pathStr;
{
    int n = 0;

    while ((pathStr = strchr(pathStr, ':')) != NULL) {
	n++;
	pathStr++;
    }
    return(++n);
}

/* 8/2/89 7/23/89 4/28/88 2/27/88
Read the palette list from an entry in .Xdefaults, if no list exists,
then use the hardcoded default.  The list is specified as a colon
separated string of palette pathnames that can be relative to the
current VEM directory.  (The paths are relative to the current VEM
directory because facets are opened on the VEM side of the IPC
interface.)
Caveats: Should only be called once at the beginning of the program.
*/
boolean
OpenPaletteInit()
{
    char *a, *b, *copy, *techDir, buf[MAXPATHLEN];
    int i;

/* dots in resource names no longer work??? */
/*    sprintf(buf, "%s.palettes", UAppName); */
    sprintf(buf, "%sPalettes", UAppName);
    if ((b = RPCXGetDefault("vem", buf)) == NULL) {
	b = defaultPalettes();
    }
    palettes_n = ListLength(b) + 1;
    palettes = (char **) malloc(palettes_n * sizeof(char *));
    copy = malloc((strlen(b) + 1)*sizeof(char));
    strcpy(copy, b);
    a = SkipWhite(copy);
    i = 0;
    while (TRUE) {
	b = strchr(a, ':');
	if (b == NULL) {
	    palettes[i++] = a;  /* one last time */
	    break;
	}
	*b = '\0';
	palettes[i++] = a;
	a = SkipWhite(++b);
    }
    /* tack on system palette at the end */
    ERR_IF1(!UGetFullTechDir(&techDir));
    sprintf(buf, "%s/system", techDir);
    ERR_IF1(!StrDup(&(palettes[i]), buf));

    /* init data structure for dialog box... */
    items = (dmWhichItem *) malloc((palettes_n) * sizeof(dmWhichItem));
    for (i = 0; i < palettes_n; i++) {
	items[i].itemName = palettes[i];
	items[i].userData = NULL;
	items[i].flag = 0;
    }
    return (TRUE);
}

/* 7/23/89 4/28/88 2/88
Uses global palettes variable.
*/
int 
RpcOpenPalette(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    int i;
    octObject facet;

    ViInit("open-palette");
    ErrClear();
    if (dmMultiWhich("Palettes", palettes_n, items, NULL, NULL) != VEM_OK) {
	PrintCon("Aborted entry");
	ViDone();
    }
    for (i = 0; i < palettes_n; i++) {
	if (items[i].flag != 0) {
	    if (ohOpenFacet(&facet, palettes[i], "schematic", "contents", "r")
		< 0) {
		PrintErr(octErrorString());
		ViDone();
	    }
	    vemOpenWindow(&facet, NULL);
	}
    }
    ViDone();
}

/***** End of RpcOpenPalette routines */

int 
RpcEditComment(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet, obj;
    vemStatus status;
    char facetHandle[16];

    ViInit("edit-comment");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }

    status = vuFindSpot(spot, &obj, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
	PrintCon("Aborted");
        ViDone();
    } else if (status == VEM_CANTFIND) {
	/* This should not be necessary, according to the manual, but it
	    doesn't seem to work without it.
	*/
	obj = facet;
    }
    /* At this point, obj is either an instance or the facet */
    ptkOctObj2Handle(&obj,facetHandle);

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkEditText ",
		   "\"Edit Comment\" ",
                   "\"ptkSetStringProp ", facetHandle, " comment %s\" ",
                   "[ptkGetStringProp ", facetHandle, " comment]",
                   (char *)NULL) )

    ViDone();

}

int
RpcEditDomain(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet;
    char facetHandle[16];

    ViInit("edit-domain");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    ptkOctObj2Handle(&facet,facetHandle);

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkChooseOne ",
                   "[ptkGetDomainNames ", facetHandle, "]  ",
                   " \"ptkSetDomain ", facetHandle, " %s \" ",
                   (char *)NULL) )
    ViDone();
}

/* Rewritten for Tcl/Tk 1/94 by Alan Kamas */
int
RpcEditSeed(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("edit-seed");
    ErrClear();
    FindClear();

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkEditStrings ",
                   " \"Set Seed\" ",
                   " \"ptkSetSeed %s \" ",
                   " \"{{Seed for Random Number} [ptkGetSeed]}\" ",
                   (char *)NULL) )
    ViDone();
}

/* Rewritten 12/17/93 for Tk/Tcl by Alan Kamas */
int
RpcEditTarget(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet;
    char facetHandle[16];

    ViInit("edit-target");
    ErrClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    ptkOctObj2Handle(&facet,facetHandle);

    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp,"ptkChooseOne ", 
                   "[ptkGetTargetNames ", facetHandle, "]  ", 
                   " \"ptkEditParams ", facetHandle, " %s Target \" ",
                   (char *)NULL) )
    ViDone();
}

