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
 Date of creation: July 1993

This file implements a class that adds Ptolemy-specific Oct/Vem commands to
a Tcl interpreter.  

**************************************************************************/

#ifndef _POct_h
#define _POct_h 1

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#include "tcl.h"
#include "TclObj.h"
#include "StringList.h"

#if defined(hppa)
/* Include math.h outside of extern "C", other wise we get errors with
   pow() g++2.5.8 */
#include <math.h>
#endif

extern "C" {
#define Pointer screwed_Pointer
#include "oct.h"  /* Oct Pointer Structure */
#include "paramStructs.h"
#include "ganttIfc.h"
#undef Pointer
}

#ifdef __GNUG__
#pragma interface
#endif

class POct : public TclObj {

public:
        // the active Tcl interpreter, for error reporting.
        static Tcl_Interp* activeInterp;

        POct(Tcl_Interp* interp = 0);
        ~POct();

        // the dispatcher is called by Tcl to handle all extension
        // commands.
        static int dispatcher(ClientData,Tcl_Interp*,int,char*[]);

// the following are the Tcl-callable functions.  Each returns TCL_OK
// or TCL_ERROR, and may set the Tcl result to return a string using
// Tcl_SetResult, Tcl_AppendElement, POct::result or POct::staticResult.
        int ptkCompile (int argc,char** argv);
        int ptkGetParams (int argc,char** argv);
        int ptkSetParams (int argc,char** argv);
        int ptkSetFindName (int argc,char** argv);
        int ptkGetMkStar (int argc,char** argv);
        int ptkSetMkStar (int argc,char** argv);
        int ptkSetRunUniverse (int argc,char** argv);
        int ptkGetStringProp (int argc,char** argv);
        int ptkSetStringProp (int argc,char** argv);
        int ptkGetMkSchemIcon (int argc,char** argv);
        int ptkSetMkSchemIcon (int argc,char** argv);
        int ptkGetSeed (int argc,char** argv);
        int ptkSetSeed (int argc,char** argv);
        int ptkGetDomainNames (int argc,char** argv);
        int ptkSetDomain (int argc,char** argv);
        int ptkGetTargetNames (int argc,char** argv);
        int ptkGetTargetParams (int argc,char** argv);
        int ptkSetTargetParams (int argc,char** argv);
        int ptkFacetContents (int argc,char** argv);
        int ptkOpenMaster (int argc,char** argv);
        int ptkOpenFacet (int argc,char** argv);
	int ptkCloseFacet (int argc,char** argv);
        int ptkIsStar (int argc,char** argv);
        int ptkIsGalaxy (int argc,char** argv);
        int ptkIsBus (int argc,char** argv);
        int ptkIsDelay (int argc,char** argv);
        int ptkGetRunLength (int argc,char** argv);
        int ptkSetRunLength (int argc,char** argv);
        int ptkSetEventLoop (int argc,char** argv);

private:
        // these three functions are used to associate POct objects
        // with interpreters.
        static POct* findPOct(Tcl_Interp*);
        void makeEntry();
        void removeEntry();

	// function to register extensions with the Tcl interpreter
	void registerFuncs();

	// State functions to remind users of past input vaules
	// Note that these are only to help out the user as a convenience
	//
	// For ptkSetSeed and ptkGetSeed
        int OldRandomSeed; 	
	// For ptkGetMkStar and ptkSetMkStar
	StringList MkStarName; 
        StringList MkStarDomain; 
	StringList MkStarDir; 
	StringList MkStarPalette;
	// For ptkSetMkSchemIcon and ptkGetMkSchemIcon
	// FIXME:  Add following line back for MkSchemIcon state
	// StringList MkSchemPalette;

	// Helper Functions that are not TCL callable directly

	// Sets Bus Parameters in the Oct data base
	// Works by first getting a prop from the oct data base, then modifying
	// it and finally sending it back.
	int SetBusParams( octObject*, ParamListType* );

	// Sets Delay Parameters in the Oct data base
	// Works by first getting a prop from the oct data base, then modifying
	// it and finally sending it back.
	int SetDelayParams( octObject *, ParamListType *);

	// Deletes all of the elements of the passed pList
	void DeletePList( ParamListType* );

	// Converts a parameter list string of the form:
	//  {name1 type1 value1} {name2 type2 value2} ...
	// into the pList format.  Allocates space for the pList
	// elements as well.  Note that all of the elements must
	// be freed once the pList is no longer needed.  DeletePList
	// does this.
	int MakePList( char* , ParamListType* );

};

#endif		// _POct_h

