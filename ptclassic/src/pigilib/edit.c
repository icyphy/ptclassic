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

/* EditDelayParams, EditBusParams
Edit the delay value associated with a delay instance,
or the bus width value associated with a bus instance.
Inputs: instPtr = delay instance pointer.
Outputs: return = TRUE if no error.
editBusOrDelay is the common part.
*/

static boolean editBusOrDelay(prop,msg,msg2)
octObject* prop;
char* msg;
char* msg2;
{
    static dmTextItem item = {NULL, 1, 10, NULL, NULL};
    char buf[100];
    int l;
    StringizeProp(prop, buf, 100);
    item.itemPrompt = msg;
    item.value = buf;
    l = strlen(buf);
    item.cols = 10;
    if (l >= 10) item.cols = l + 1;
    if (dmMultiText(msg2, 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        return(TRUE);
    }
    prop->contents.prop.type = OCT_STRING;
    prop->contents.prop.value.string = item.value;
    IntizeProp(prop);
    (void) octModify(prop);
    return(TRUE);
}

/* EditDelayParams no longer needed - aok */
/* EditBusParams no longer needed - aok */
/* EditSogParams no longer needed - aok */


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

/* 4/14/89 3/19/89
Edit the formal param list of a galaxy schematic facet.
Returns 0 if there is an error.
Returns 1 if the parameters were changed or if the user clicked OK.
Returns 2 if the command was aborted by the user.
*/
static int
EditFormalParamList(galFacetPtr)
octObject *galFacetPtr;
{
static dmTextItem defaultItem = {NULL, 1, dmIncrement, NULL, NULL};
    int i, j, itemsN, paramsN;
    int width, maxwidth;
    ParamListType pList;
    ParamType *place;
    dmTextItem *items;

    ERR_IF1(!GetFormalParams(galFacetPtr, &pList));

    width = 0;
    maxwidth = 0;

    /* put formal params (if any) into item list */
    itemsN = (pList.length + 1) * 3;
    items = (dmTextItem *) calloc((unsigned)itemsN, sizeof(dmTextItem));
    place = pList.array;
    j = 0;
    for (i = 0 ; i < pList.length; i++) {
	    /* add in the name, type, value of each param */
	items[j] = defaultItem;
	items[j].itemPrompt = "Name";
	items[j++].value = place->name;
	width = strlen(place->name);
	if(maxwidth<width) maxwidth = width;
	items[j] = defaultItem;
	items[j].itemPrompt = "Type";
	items[j++].value = place->type ? place->type : "FLOAT";
	items[j] = defaultItem;
	items[j].itemPrompt = "Value";
	items[j++].value = place->value;
	width = strlen(place->value);
	if(maxwidth<width) maxwidth = width;
	place++;
    }
    items[j] = defaultItem;
    items[j].itemPrompt = "New name";
    items[j++].value = "";
    items[j] = defaultItem;
    items[j].itemPrompt = "New type";
    items[j++].value = "";
    items[j] = defaultItem;
    items[j].itemPrompt = "New value";
    items[j++].value = "";
    /* set the width */
    maxwidth += dmIncrement;
    for (i = 0 ; i < itemsN; i++) {
	items[i].cols = maxwidth;
    }
    if (dmMultiText("Edit Formal Parameters", itemsN, items) != VEM_OK) {
	PrintCon("Aborted entry");
	return(2);
    }
    /* Tranfer params from item list to pList, deleting blank entries.
	Assumes there is an extra empty slot in pList.array (kludgy).
	See paramStructs.c.
    */
    place = pList.array;
    j = 0;
    paramsN = 0;
    for (i = 0; i < pList.length + 1; i++) {
	if (strlen(SkipWhite(items[j].value)) != 0) {
	    /* copy this name-type-value triplet */
	    place->name = items[j++].value;
	    place->type = items[j++].value;
	    place->value = items[j++].value;
	    place++;
	    paramsN++;
	} else {
	    /* skip this name-type-value triplet */
	    j += 3;
	}
    }
    pList.length = paramsN;
    free(items);

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
    static dmTextItem item = {"Comment", 1, 80, NULL, NULL};
    char *comment;
    octObject facet, obj;
    vemStatus status;

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
    if (!GetCommentProp(&obj, &comment)) {
	PrintErr(ErrGet());
        ViDone();
    }
    item.value = (comment == NULL) ? "" : comment;
    if (dmMultiText("Edit Comment", 1, &item) != VEM_OK) {
	PrintCon("Aborted entry");
        ViDone();
    }
    if (!SetCommentProp(&obj, item.value)) {
	PrintErr(ErrGet());
        ViDone();
    }
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

/* kernel Call */
void KcEditSeed();

int
RpcEditSeed(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    int n;
    char buf[64];
    static oldN = 1;
    static dmTextItem item = {"Seed for Random number", 1, 20,
				NULL, NULL};

    ViInit("edit-seed");
    ErrClear();
    FindClear();

    /* default seed is 1 */
    sprintf(buf, "%d", oldN);
    item.value = buf;
    if (dmMultiText("Edit-Seed", 1, &item) != VEM_OK) {
            PrintCon("Aborted entry");
            PrintErr(ErrGet());
	    ViDone();
    }
    ERR_IF2((n = atoi(item.value)) <= 0,
        "Invalid entry, number must be > 0");

    oldN = n;
    KcEditSeed(n);
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

