static const char file_id[] = "TclStarIfc.cc";

/**************************************************************************
Version identification:
$Id$

Defines a simple interface for stars that call Tcl functions.
Makes the star parameters available to Tcl scripts.

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

**************************************************************************/

#include "Block.h"
#include "TclStarIfc.h"
extern "C" {
#include "ptk.h"
}

/////////////////////////////////////////////////////////////////////////
//			Tcl Callable Procedures
//
// Define the callback procedure used by Tcl to get the value of
// the inputs.
static int grabInputs(
    ClientData tcl,                     // Pointer to the Tcl interface
    Tcl_Interp*,                 	// Current interpreter
    int,                           	// Number of arguments
    char*[]                         	// Argument strings
) {
	InfString inputs = ((TclStarIfc*)tcl)->getInputs();
	Tcl_SetResult(ptkInterp,(char*)inputs, TCL_VOLATILE);
	return TCL_OK;
}

// Define the callback procedure used by Tcl to get the state of
// the inputs.
static int grabInputsState(
    ClientData tcl,                     // Pointer to the Tcl interface
    Tcl_Interp*,                 	// Current interpreter
    int,                           	// Number of arguments
    char*[]                         	// Argument strings
) {
	InfString inputs = ((TclStarIfc*)tcl)->getInputsState();
	Tcl_SetResult(ptkInterp,(char*)inputs, TCL_VOLATILE);
	return TCL_OK;
}

// Define the callback procedure used by Tcl to set the value of
// the outputs.
static int setOutputs(
    ClientData tcl,                     // Pointer to the Tcl interface
    Tcl_Interp*,                 	// Current interpreter
    int argc,                           // Number of arguments
    char *argv[]      			// Argument strings
) {
	float temp;
	for(int i=0; i < (argc-1); i++) {
	    if(sscanf(argv[i+1], "%f", &temp) != 1) {
	        Error::abortRun(*(((TclStarIfc*)tcl)->myStar),
		    "Invalid output value from Tcl to star");
		return TCL_ERROR;
	    }
	    ((TclStarIfc*)tcl)->setOneOutput(i,temp);
	}
        return TCL_OK;
}


/////////////////////////////////////////////////////////////////////////
//			Methods for TclStarIfc class
//
// constructor
TclStarIfc::TclStarIfc () {
	starID = "tclStar";
	starID += unique++;
	inputNewFlags = NULL;
	outputValues = NULL;
	outputNewFlags = NULL;
	outputArraySize = 0;
	inputArraySize = 0;
}

// destructor
TclStarIfc::~TclStarIfc() {
	// exit immediately if there is no ptk interpreter.  This happens
	// if there are global TclStarIfc objects in the program but the
	// interpreter failed to set up.
	if (!ptkInterp) return;

	InfString buf;

	// First, invoke the Tcl desctructor procedure, if it was defined
	buf = "info procs destructorTcl_";
	buf += starID;
	if(Tcl_GlobalEval(ptkInterp, (char*)buf) == TCL_OK &&
	    strlen(ptkInterp->result) != 0)
	    	callTclProc("destructorTcl");

	// Delete Tcl commands created for this star
	buf = "grabInputs_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "grabInputsState_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "setOutputs_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "goTcl_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "wrapupTcl_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "destructorTcl_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	Tcl_UnsetVar(ptkInterp,(char*)starID,TCL_GLOBAL_ONLY);
	LOG_DEL; delete [] inputNewFlags;
	LOG_DEL; delete [] outputValues;
	LOG_DEL; delete [] outputNewFlags;
}

int TclStarIfc::setup (Block* star,
		       int numInputs,
		       int numOutputs,
		       const char* tcl_file) {
	InfString buf;

	myStar = star;

	if(Tcl_SetVar(ptkInterp, "starID", (char*)starID, TCL_GLOBAL_ONLY)
			== NULL) {
		Error::abortRun(*star, "Failed to set starID");
		return FALSE;
	}

	if (numInputs > 0) {
		buf = "grabInputs_";
		buf += starID;
	        Tcl_CreateCommand(ptkInterp, (char*)buf,
		    grabInputs, (ClientData)this, NULL);
	}

	if (numInputs > 0) {
		buf = "grabInputsState_";
		buf += starID;
	        Tcl_CreateCommand(ptkInterp, (char*)buf,
		    grabInputsState, (ClientData)this, NULL);
	}

	if (numOutputs > 0) {
		buf = "setOutputs_";
		buf += starID;
	        Tcl_CreateCommand(ptkInterp, (char*)buf,
		    setOutputs, (ClientData) this, NULL);
	}

	// Set entries in the $starID array
	buf = numInputs;
	Tcl_SetVar2(ptkInterp, (char*)starID, "numInputs",
			(char*)buf, TCL_GLOBAL_ONLY);

	buf = numOutputs;
	Tcl_SetVar2(ptkInterp, (char*)starID, "numOutputs",
			(char*)buf, TCL_GLOBAL_ONLY);

	// Step through all the states
	State* s;
	BlockStateIter next(*star);
	while ((s = next++) != 0) {
		InfString val = s->currentValue();
		InfString name = s->name();
		Tcl_SetVar2(ptkInterp, (char*)starID, (char*)name,
			(char*)val, TCL_GLOBAL_ONLY);
	}

	if (numInputs > 0) {
		LOG_DEL; delete [] inputNewFlags;
		LOG_NEW; inputNewFlags = new int[numInputs];
		// Initialize the outputs
		for (int i = 0; i < numOutputs; i++)
		  inputNewFlags[i] = FALSE;
	}
	inputArraySize = numInputs;

	if (numOutputs > 0) {
		LOG_DEL; delete [] outputValues;
		LOG_DEL; delete [] outputNewFlags;
		LOG_NEW; outputValues = new double[numOutputs];
		LOG_NEW; outputNewFlags = new int[numOutputs];
		// Initialize the outputs
		for (int i = 0; i < numOutputs; i++) {
		  outputValues[i] = 0.0;
		  outputNewFlags[i] = FALSE;
		}
	}
	outputArraySize = numOutputs;

	if(tcl_file[0] == '$') {
	        buf = "source [ptkExpandEnvVar \\";
		buf += tcl_file;
		buf += "]";
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    Tcl_GlobalEval(ptkInterp, "ptkDisplayErrorInfo");
		    Error::abortRun(*star, "Cannot source tcl script");
		    return FALSE;
		}
	} else {
	        buf = "source ";
		buf += tcl_file;
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    Tcl_GlobalEval(ptkInterp, "ptkDisplayErrorInfo");
		    Error::abortRun(*star, "Cannot source tcl script");
		    return FALSE;
	        }
	}

	// Determine whether the star is synchronous by checking to
	// see whether the goTcl procedure has been defined by the
	// Tcl script.
	buf = "info procs goTcl_";
	buf += starID;
	if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK ||
	strlen(ptkInterp->result) == 0)
			synchronous = 0;
	else synchronous = 1;

	// Determine whether Tk has been loaded into the system
	// (if so, then its event loop will need to be updated).
	Tcl_GlobalEval( ptkInterp, "info exists tk_library" );
	if (*(ptkInterp->result)=='1') tkExists = 1 ; else tkExists = 0;

	return TRUE;
}

int TclStarIfc::callTclProc(const char* name) {
	InfString buf;
	buf = "info procs ";
	buf += name;
	buf += "_";
	buf += starID;
	if(Tcl_GlobalEval(ptkInterp, (char*)buf) == TCL_OK &&
	    strlen(ptkInterp->result) != 0) {
		// A name_starID procedure is defined.  Invoke it.
	        if(Tcl_SetVar(ptkInterp, "starID", (char*)starID,
			TCL_GLOBAL_ONLY) == NULL) {
		    Error::abortRun(*myStar, "Failed to set starID");
		    return FALSE;
	        }
		buf = name;
		buf += "_";
		buf += starID;
		buf += " ";
		buf += starID;
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    Tcl_GlobalEval(ptkInterp, "ptkDisplayErrorInfo");
		    Error::abortRun(*myStar, "Failed to run Tcl procedure");
		    return FALSE;
		}
	    }
	return TRUE;
}

int TclStarIfc::go () {

    // If the tk library is part of the interp, update the graphics
    if (tkExists) Tcl_Eval( ptkInterp, "update");

    // If the there exists a "goTcl" procedure, call it
    if(synchronous) return callTclProc("goTcl");
    else return TRUE;
}

int TclStarIfc::wrapup() {
    InfString buf;
    buf = "info procs wrapupTcl_";
    buf += starID;
    if(Tcl_GlobalEval(ptkInterp, (char*)buf) == TCL_OK &&
	strlen(ptkInterp->result) != 0)
	    // A name_starID procedure is defined.  Invoke it.
	    return callTclProc("wrapupTcl");
    else return TRUE;
}

// Convert values on input to strings; works for all Ptolemy domains
InfString TclStarIfc::getInputs () {
	BlockPortIter nexti(*myStar);
	PortHole *p;
	InfString result;
	while ((p = nexti++) != 0) {
	    // return a quoted string for Tcl consumption
	    if (p->isItOutput()) continue;
	    result += "{";
	    result += ((*p)%0).print();
	    result += "} ";
	}
	return result;
}

// Convert values on input to strings; works for all Ptolemy domains
InfString TclStarIfc::getInputsState () {
    	InfString result;
	for (int i = 0; i<inputArraySize; i++)
	    result << inputNewFlags[i] << " ";
	return result;
}

// Sets one element of inputNewFlags array to flag (either TRUE or FALSE)
void TclStarIfc::setOneNewInputFlag(int inNum, int flag) {
	if ( (inNum >= inputArraySize) || (inNum < 0) ) {
	    Error::warn(*myStar, "Input port number is out of range");
	}
	else {
	    inputNewFlags[inNum] = flag;
	}
}

// Sets all elements of inputNewFlags array to flag (either TRUE or FALSE)
void TclStarIfc::setAllNewInputFlags (int flag) {
	for ( int port = 0; port < inputArraySize; port++ )
	  inputNewFlags[port] = flag;
}

// Load the local buffer outputValues[] with values supplied by Tcl
void TclStarIfc::setOneOutput (int outNum, double outValue) {
	if ( outNum >= outputArraySize ) {
	    Error::warn(*myStar, "Too many outputs supplied by Tcl");
	}
	else if ( outNum < 0 ) {
	    Error::warn(*myStar, "Negative output port number");
	}
	else {
	    outputValues[outNum] = outValue;
	    outputNewFlags[outNum] = TRUE;
	}
}

// Sets all elements of outputNewFlags array to flag (either TRUE or FALSE)
void TclStarIfc::setAllNewOutputFlags (int flag) {
	for ( int port = 0; port < outputArraySize; port++ )
	  outputNewFlags[port] = flag;
}

// Sets one element of outputNewFlags array to flag (either TRUE or FALSE)
void TclStarIfc::setOneNewOutputFlag(int outNum, int flag) {
	if ( (outNum >= outputArraySize) || (outNum < 0) ) {
	    Error::warn(*myStar, "Output port number is out of range");
	}
	else {
	    outputNewFlags[outNum] = flag;
	}
}

// Initialize the static counter for unique names.
// The initialization occurs at load time.
int TclStarIfc::unique = 0;
