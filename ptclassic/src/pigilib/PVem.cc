/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
		       
 Programmer:  Alan Kamas
 Date of creation: October 1993

This file implements a class that adds Ptolemy-specific Vem commands to
a Tcl interpreter.  

**************************************************************************/
static const char file_id[] = "PVem.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "PVem.h"

#include <stdio.h>
#include <string.h>

extern "C" {
#define Pointer screwed_Pointer
#include "oct.h"  // Oct Pointer Structure
#include "handle.h"
#include "vemInterface.h"
#include "octMacros.h"
#include "rpc.h"
#undef Pointer
}

// FIXME: This include is only needed for the "quote" macro
//        Seems silly to include so much extra baggage - aok
#include "isa.h"


// we want to be able to map Tcl_interp pointers to PVem objects.
// this is done with a table storing all the PVem objects.
// for now, we allow up to MAX_PVem PVem objects at a time.
const int MAX_PVem = 10;

static PVem* ptable[MAX_PVem];

/////////////////////////////////////////////////////////////////////


// this is a static function.
PVem* PVem::findPVem(Tcl_Interp* arg) {
        for (int i = 0; i < MAX_PVem; i++)
                if (ptable[i]->interp == arg) return ptable[i];
        return 0;
}

void PVem::makeEntry() {
        int i = 0;
        while (ptable[i] != 0 && i < MAX_PVem) i++;
        if (i >= MAX_PVem) return;
        ptable[i] = this;
}

void PVem::removeEntry() {
        for (int i = 0; i < MAX_PVem; i++) {
                if (ptable[i] == this) {
                        ptable[i] = 0;
                }
        }
}

/////////////////////////////////////////////////////////////////////


// constructor
PVem::PVem(Tcl_Interp* i) 
{
        interp = i;
	// FIXME:  check for validity of interpreter here
	//         may want to create interp if pointer invalid
        //         if so, set myInterp True
        myInterp = FALSE;
        makeEntry();
	registerFuncs();
}

// destructor
PVem::~PVem() {
        removeEntry();
        if (myInterp) {
                Tcl_DeleteInterp(interp);
                interp = 0;
        }
}

/////////////////////////////////////////////////////////////////////////

// Helper functions used by the TCL callable fuctions below


// AdjustScalePan
// This function is only used in pvOpenWindow.  Since vem opens a window
// on a facet having no geometry with a very inconvenient scale and annoying
// cross-hair(X-Y axes), we do an automatic zoom-out  to an appropriate scale
// and pan to a point far from the origin so that the cross hair won't be seen
// in the window. 
//
// This function was taken directly from "misc.c" in the pigilib src
// directory.

void PVem::AdjustScalePan(Window window, octId facetId) {
    RPCSpot spot;
    lsList argList;
    RPCArg boxArg;
    RPCPointArg bbPoints[2];
    char *command;

    spot.theWin = window;
    spot.facet = facetId;
    // center point of window after panning, should be far from origin.
    spot.thePoint.x = 1000;
    spot.thePoint.y = 1000;
    argList= lsCreate();
    boxArg.argType = VEM_BOX_ARG;
    boxArg.length = 1;
    bbPoints[0].theWin = window;  bbPoints[1].theWin = window;
    bbPoints[0].facet = facetId;  bbPoints[1].facet = facetId;
    // zoom-out to box (-1,-1)(1,1).  Since vem opens a window on a empty facet
    // with corner points at (-22,22)(22,22),  the zoom-out factor is about
    // 22 times and is appropriate for constructing ptolemy schematic
    // diagrams.  
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


/////////////////////////////////////////////////////////////////////////

// Tcl callable fuctions:

// pvOpenWindow <OctInstanceHandle> 
// Opens a new Vem window for the passed Oct Obj. 
// This code is modified from misc.c RpcOpenFacet
// - Alan Kamas 9/93
int PVem::pvOpenWindow (int aC,char** aV) {

    if (aC != 2) return usage ("pvOpenWindow <OctInstanceHandle>");

    octObject facet;
    if (ptkHandle2OctObj(aV[1], &facet) == 0) {
        Tcl_AppendResult(interp, "Bad or Stale Facet Handle passed to ", aV[0],
                         (char *) NULL);
        return TCL_ERROR;
    }

    Window newWindow;  // the vem window looking at facet 
    newWindow = vemOpenWindow(&facet, NULL);
    // if the facet has no bounding box, that means it has no geometry
    // to define a bounding box, then we zoom-out to the appropriate scale
    // and pan to let the X-Y axes disappearing from the window.
    octBox bbox;  //  an argument to octBB, return value not used
    if (octBB(&facet, &bbox) == OCT_NO_BB) { 
       AdjustScalePan(newWindow, facet.objectId);
    }
    return TCL_OK;
}

////////////////////////////////////////////////////////////////////////

// An InterpFuncP is a pointer to a Tcl Obj function that takes an argc-argv
// argument list and returns TCL_OK or TCL_ERROR.

typedef int (PVem::*InterpFuncP)(int,char**);

struct InterpTableEntry {
        char* name;
        InterpFuncP func;
};

// These macros define entries for the table
#define ENTRY(verb) { quote(verb), PVem::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), PVem::action }

// Here is the table.  Make sure it ends with "0,0"
static InterpTableEntry funcTable[] = {
	ENTRY(pvOpenWindow),
	{ 0, 0 }
};

// register all the functions.
void PVem::registerFuncs() {
	int i = 0;
	while (funcTable[i].name) {
		Tcl_CreateCommand (interp, funcTable[i].name,
				   PVem::dispatcher, (ClientData)i, 0);
		i++;
	}
}

// static member: tells which Tcl interpreter is "innermost"
Tcl_Interp* PVem::activeInterp = 0;

// dispatch the functions.
int PVem::dispatcher(ClientData which,Tcl_Interp* interp,int argc,char* argv[])
			   {
	PVem* obj = findPVem(interp);
	if (obj == 0) {
		strcpy(interp->result,
		       "Internal error in PVem::dispatcher!");
		return TCL_ERROR;
	}
	int i = int(which);
	// this code makes an effective stack of active Tcl interpreters.
	Tcl_Interp* save = activeInterp;
	activeInterp = interp;
	int status = (obj->*(funcTable[i].func))(argc,argv);
	activeInterp = save;
	return status;
}
