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
/* misc.c  edg
Version identification:
 $Id$
*/

/*
        Added ShowFacetNum to show the "handle" of the current facet.
        this function is for debugging of the tcl/tk interface.
        - aok  2/22/93
*/

/* Includes */
#include "local.h"
#include <stdio.h>
#include <strings.h>
#include "rpc.h"
#include "vemInterface.h"
#include "util.h"
#include "octIfc.h"
#include "octMacros.h"
#include "err.h"
#include "main.h"
#include "list.h"

#define dmWidth 40


char* callParseClass();

/* uses the ptman script to print it out the man page for the given
   star or galaxy */
boolean
ManPage(starName)
char *starName;
{
    char buf[612];

    sprintf(buf, "ptman -x %s &", starName);
    PrintDebug(buf);
    if (util_csystem(buf)) {
        sprintf(buf, "Cannot find man page for Ptolemy code file '%s'", 
                starName);
        ErrAdd(buf);
        return (FALSE);
    }
    return (TRUE);
}

/* print the manual entry of the star the cursor is currently over.
   written by Alan Kamas.  Based on the "LookInside" code. */
int 
Man(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject mFacet, inst, facet;
    vemStatus status;
    char *fullName, codeFile[512], domain[64], *base, *period;

    ViInit("Man");
    ErrClear();

    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
        PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
        PrintErr("Cursor must be over an icon instance");
        ViDone();
    } else {
        if (IsGal(&inst) || IsUniv(&inst) || IsPal(&inst)) {
            PrintErr("The Man command is currently only supported for Stars.");
            ViDone();
        } else {
            if (IsStar(&inst)) {
                if (!MyOpenMaster(&mFacet, &inst, "interface", "r")) {
                    PrintErr(ErrGet());
                    ViDone();
                }
                octFullName(&mFacet, &fullName);

                /* Figure out source file name */
                if (!IconFileToSourceFile(fullName, codeFile, domain)) {
                    PrintErr(ErrGet());
                    ViDone();
                }
		/* Convert source file name to base name for look up */
                base = BaseName(codeFile); 
		/* Only want base part, nothing after the period */
		if ((period = strchr(base, '.')) != NULL) { *period = '\0'; }

		/* now, call ptman with the base star name. */
                if (!ManPage(base)){
                    PrintErr(ErrGet());
                    ViDone();
		}

            } else {
                PrintErr("The Man command is currently only supported for Stars.");
                ViDone();
            }
        }
    }
    ViDone();
}


/* ShowFacetNum prints the number of the facet to the standard out.
   This is a helping function for the development of the TCL/TK interface
   to Ptolemy - aok */
int
ShowFacetNum (spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    octObject facet, inst;
    char FacetName[64], InstanceName[64];
    vemStatus status;

    ViInit("show facet number");
    ErrClear();

    /* Get the facet based on the "spot" */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
        PrintErr(octErrorString());
        ViDone();
    }
    sprintf( FacetName, "OctObj%x", (long)facet.objectId);

    /* Get the instance under the cursor */
    status = vuFindSpot(spot, &inst, OCT_INSTANCE_MASK);
    if (status == VEM_NOSELECT) {
        PrintCon("Aborted");
        ViDone();
    } else if (status != VEM_OK) {
        /* cursor not over an instance... */
        sprintf (InstanceName, "NIL");
    } else {
        /* cursor is over some type of instance... */
        sprintf( InstanceName, "OctObj%x", (long)inst.objectId);
    }

    printf("Facet_Handle Instance_Handle: %s %s\n", FacetName, InstanceName);

    ViDone();
}




/***********************************************************************
PrintFacet(): calls "prfacet" utility to save PostScript to printer
or file.
***********************************************************************/
int
PrintFacet(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	static dmTextItem items[] = {
		{"oct2ps options:", 1, 40, "-x -X -b 3x3", NULL},
		{"(P)rint or to (F)ile:", 1, 40, "P", NULL},
		{"file name:", 1, 40, "", NULL}
	};
#define ITEMS_N sizeof(items) / sizeof(dmTextItem)

	char buf[512];
	octObject facet;
	char* fullName;
	char fileName[128];

/** Begin here. **/
	ViInit("print facet");
	ErrClear();

/** Get facet information. **/
	facet.objectId = spot->facet;
	if (octGetById(&facet) != OCT_OK) {
		PrintErr(octErrorString());
		ViDone();
	}
	octFullName(&facet, &fullName);

/** Display dialog box. **/
	if (dmMultiText("print facet (PRINTER variable must be set)",
			ITEMS_N, items) != VEM_OK) {
		PrintCon("Aborted entry");
		ViDone();
	}

/** Error-check the user input. **/
	if ((items[1].value[0] == 'f') || (items[1].value[0] == 'F')) {
		fileName[0] = '\000'; /* Clear previous value. */
		sscanf(items[2].value, "%s", fileName); /* Get first word. */
		if (!fileName[0]) {
			PrintErr("print-facet needs a filename");
			ViDone();
		}
		sprintf(buf, "prfacet -TOFILE %s %s %s", fileName,
				items[0].value, fullName);
	} else {
		sprintf(buf, "prfacet %s %s", items[0].value, fullName);
	}

/** Call the prfacet command. **/
	PrintDebug(buf);
	if (util_csystem(buf)) {
		sprintf(buf, "Error invoking prfacet utility.");
		PrintErr(buf);
	}
	ViDone();

#undef ITEMS_N
} /* end PrintFacet() */


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

static void AdjustScalePan();

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


    ViInit("open-facet");
    ErrClear();

    /* see if the facet name was given as a Vem argument */
    if ((lsLastItem(cmdList, (lsGeneric *) &theArg, LS_NH) == VEM_OK) &&
	(theArg->argType == VEM_TEXT_ARG)) {
	/* set default values */
	facet.contents.facet.view = "schematic";
	facet.contents.facet.facet = "contents";
	facet.objectId = 0;	/* silence Purify */
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
