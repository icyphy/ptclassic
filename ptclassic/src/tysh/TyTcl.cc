/**************************************************************************
Version identification:
$Id$
		       
    Programmer:  Edward A. Lee (based on code by Alan Kamas and Joe Buck)

    This file implements in a class C++ extensions to Tcl used
    in the Tycho system. The constructor registers these
    commands with the interpreter passed as a argument.

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

**************************************************************************/
static const char file_id[] = "TyTcl.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "TyTcl.h"
#include "tk.h"

// FIXME: This include is only needed for the "quote" macro
//        Seems silly to include so much extra baggage - aok
#include "isa.h"


// we want to be able to map Tcl_interp pointers to TyTcl objects.
// this is done with a table storing all the TyTcl objects.
// for now, we allow up to MAX_TyTcl TyTcl objects at a time.
const int MAX_TyTcl = 10;

static TyTcl* ptable[MAX_TyTcl];

/////////////////////////////////////////////////////////////////////////
// FIXME: What we really want here is a way to check a font to see
// whether it exists. Here, we actually allocate the font.  This
// implies that we should also free it later. So this function is
// not used for now.
//
// Tcl callable functions.  If you add functions here, be sure to
// add them to the TyTcl.h file and the funcTable below.

int TyTcl::ptkCheckFont(int argc, char** argv) {

  if (argc != 2) return usage ("ptkCheckFont <fontName>");

  XFontStruct *xf = Tk_GetFontStruct(interp, tkwin, argv[1]);
  if (!xf) {
    result("");
  } else {
    result(Tk_NameOfFontStruct(xf));
  }
  return TCL_OK;
}

/////////////////////////////////////////////////////////////////////
// Given a pointer to an interpreter, identify the corresponding TyTcl object.
TyTcl* TyTcl::findTyTcl(Tcl_Interp* arg) {
        for (int i = 0; i < MAX_TyTcl; i++)
                if (ptable[i]->interp == arg) return ptable[i];
        return 0;
}

void TyTcl::makeEntry() {
        int i = 0;
        while (ptable[i] != 0 && i < MAX_TyTcl) i++;
        if (i >= MAX_TyTcl) return;
        ptable[i] = this;
}

void TyTcl::removeEntry() {
        for (int i = 0; i < MAX_TyTcl; i++) {
                if (ptable[i] == this) {
                        ptable[i] = 0;
                }
        }
}

/////////////////////////////////////////////////////////////////////


// constructor
TyTcl::TyTcl(Tcl_Interp* i, Tk_Window tk) 
{
        interp = i;
	tkwin = tk;
	// FIXME:  check for validity of interpreter here
	//         may want to create interp if pointer invalid
        //         if so, set myInterp True
        myInterp = 0;
        makeEntry();
	registerFuncs();
}

// destructor
TyTcl::~TyTcl() {
        removeEntry();
        if (myInterp) {
                Tcl_DeleteInterp(interp);
                interp = 0;
        }
}


////////////////////////////////////////////////////////////////////////
// An InterpFuncP is a pointer to a Tcl Obj function that takes an argc-argv
// argument list and returns TCL_OK or TCL_ERROR.

typedef int (TyTcl::*InterpFuncP)(int,char**);

struct InterpTableEntry {
        char* name;
        InterpFuncP func;
};

// These macros define entries for the table
#define ENTRY(verb) { quote(verb), TyTcl::verb }

// synonyms or overloads on argv[0]
#define ENTRY2(verb,action) { quote(verb), TyTcl::action }

// Here is the table.  Make sure it ends with "0,0"
static InterpTableEntry funcTable[] = {
	ENTRY(ptkCheckFont),
	{ 0, 0 }
};

// register all the functions.
void TyTcl::registerFuncs() {
	int i = 0;
	while (funcTable[i].name) {
		Tcl_CreateCommand (interp, funcTable[i].name,
				   TyTcl::dispatcher, (ClientData)i, 0);
		i++;
	}
}

// static member: tells which Tcl interpreter is "innermost"
Tcl_Interp* TyTcl::activeInterp = 0;

// dispatch the functions.
int TyTcl::dispatcher(ClientData which,Tcl_Interp* interp,int argc,char* argv[])
			   {
	TyTcl* obj = findTyTcl(interp);
	if (obj == 0) {
		strcpy(interp->result,
		       "Internal error in TyTcl::dispatcher!");
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

