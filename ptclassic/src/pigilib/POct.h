/**************************************************************************
Version identification:
$Id$

Copyright (c) 1993  The Regents of the University of California.
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
		       
 Programmer:  Alan Kamas
 Date of creation: July 1993

This file implements a class that adds Ptolemy-specific Oct/Vem commands to
a Tcl interpreter.  

**************************************************************************/

#ifndef _POct_h
#define _POct_h 1
#include "tcl.h"

typedef int boolean;
extern "C" {
#define Pointer screwed_Pointer
#include "oct.h"  /* Oct Pointer Structure */
#include "paramStructs.h"
#undef Pointer
}

#ifdef __GNUG__
#pragma interface
#endif

class StringList;

class POct {

private:
        // the Tcl interpreter
        Tcl_Interp* interp;

	// flag to indicate that interp is owned by me
	short myInterp;

        // FIXME:  This and a number of other functions here are 
        //         direct copies from PTcl.  It is almost never
        //         a good idea to duplicate code.  Must be some
        //         way around it. - aok

        // these three functions are used to associate POct objects
        // with interpreters.
        static POct* findPOct(Tcl_Interp*);
        void makeEntry();
        void removeEntry();

	// function to register extensions with the Tcl interpreter
	void registerFuncs();

	// return a usage error
	int usage(const char*);

	// return the passed value (with proper cleanup) as the result
	// of a Tcl Command
	int result(StringList&);
	int result(char*);
	int result(int);

	// return a "static result".  Don't give this one a stringlist!
	int staticResult(const char*);

	// append a value to the result, using Tcl_AppendElement.
	void addResult(const char*);

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
	int ptkIsStar (int argc,char** argv);
	int ptkIsGalaxy (int argc,char** argv);
	int ptkIsBus (int argc,char** argv);
	int ptkIsDelay (int argc,char** argv);
	int ptkGetRunLength (int argc,char** argv);
	int ptkSetRunLength (int argc,char** argv);
};

#endif		// _POct_h

