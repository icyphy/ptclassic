static const char file_id[] = "TclStarIfc.cc";

/**************************************************************************
Version identification:
$Id$

Defines a simple interface for stars that call Tcl functions.
Makes the star parameters available to Tcl scripts.

Copyright (c) 1993 The Regents of the University of California.
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

#include "TclStarIfc.h"

/////////////////////////////////////////////////////////////////////////
//			Tcl Callable Procedures
//
// Define the callback procedure used by Tcl to get the value of
// the inputs.
static int grabInputs(
    ClientData tcl,                     // Pointer to the Tcl interface
    Tcl_Interp *interp,                 // Current interpreter
    int argc,                           // Number of arguments
    char **argv                         // Argument strings
) {
	InfString inputs = ((TclStarIfc*)tcl)->getInputs();
	Tcl_SetResult(ptkInterp,(char*)inputs, TCL_VOLATILE);
	return TCL_OK;
}

// Define the callback procedure used by Tcl to get the value of
// the inputs.
static int setOutputs(
    ClientData tcl,                     // Pointer to the Tcl interface
    Tcl_Interp *interp,                 // Current interpreter
    int argc,                           // Number of arguments
    char **argv                         // Argument strings
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
	outputValues = NULL;
	arraySize = 0;
}

// destructor
TclStarIfc::~TclStarIfc() {
	InfString buf;
	// Delete Tcl commands created for this star
	buf = "grabInputs_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "setOutputs_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	buf = "callTcl_";
	buf += starID;
	Tcl_DeleteCommand(ptkInterp, (char*)buf);
	Tcl_UnsetVar(ptkInterp,(char*)starID,TCL_GLOBAL_ONLY);
	LOG_DEL; delete [] outputValues;
}

int TclStarIfc::setup (Block* star,
		       int numInputs,
		       int numOutputs,
		       const char* tcl_file) {
	InfString buf;

	myStar = star;

	char ncstring[] = "starID";
	if(Tcl_SetVar(ptkInterp, ncstring, (char*)starID, TCL_GLOBAL_ONLY)
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

	if (numOutputs > 0) {
		buf = "setOutputs_";
		buf += starID;
	        Tcl_CreateCommand(ptkInterp, (char*)buf,
		    setOutputs, (ClientData) this, NULL);
	}

	// Set entries in the $starID array
	buf = numInputs;
	char ncstring2[] = "numInputs";
	Tcl_SetVar2(ptkInterp, (char*)starID, ncstring2,
			(char*)buf, TCL_GLOBAL_ONLY);

	buf = numOutputs;
	char ncstring3[] = "numOutputs";
	Tcl_SetVar2(ptkInterp, (char*)starID, ncstring3,
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

	if (numOutputs > 0) {
		LOG_DEL; delete [] outputValues;
		LOG_NEW; outputValues = new double[numOutputs];
		// Initialize the outputs
		for (int i = 0; i < numOutputs; i++)
		outputValues[i] = 0.0;
	}
	arraySize = numOutputs;

	char ncstring4[] = "ptkDisplayErrorInfo";
	if(tcl_file[0] == '$') {
	        buf = "source [ptkExpandEnvVar \\";
		buf += tcl_file;
		buf += "]";
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    Tcl_GlobalEval(ptkInterp, ncstring4);
		    Error::abortRun(*star, "Cannot source tcl script");
		    return FALSE;
		}
	} else {
	        buf = "source ";
		buf += tcl_file;
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    Tcl_GlobalEval(ptkInterp, ncstring4);
		    Error::abortRun(*star, "Cannot source tcl script");
		    return FALSE;
	        }
	}

	// Determine whether the star is synchronous by checking to
	// see whether the callTcl procedure has been defined by the
	// Tcl script.
	buf = "info procs callTcl_";
	buf += starID;
	if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK ||
	strlen(ptkInterp->result) == 0)
			synchronous = 0;
	else synchronous = 1;

	return TRUE;
}

int TclStarIfc::go () {
	InfString buf;

	// If synchronous == TRUE, callTcl
	if(synchronous) {
		char ncstring5[] = "starID";
	        if(Tcl_SetVar(ptkInterp, ncstring5, (char*)starID,
			TCL_GLOBAL_ONLY) == NULL) {
		    Error::abortRun(*myStar, "Failed to set starID");
		    return FALSE;
	        }
		buf = "callTcl_";
		buf += starID;
		buf += " ";
		buf += starID;
	        if(Tcl_GlobalEval(ptkInterp, (char*)buf) != TCL_OK) {
		    char ncstring6[] = "ptkDisplayErrorInfo";
		    Tcl_GlobalEval(ptkInterp, ncstring6);
		    Error::abortRun(*myStar, "Failed to run callTcl procedure");
		    return FALSE;
	        }
	}
	return TRUE;
}

InfString TclStarIfc::getInputs () {
	BlockPortIter nexti(*myStar);
	PortHole *p;
	InfString result;
	while ((p = nexti++) != 0) {
	    // return a quoted string for tcl consumption
	    result += "{";
	    result += ((*p)%0).print();
	    result += "} ";
	}
	return result;
}

// Load the local buffer outputValues[] with values supplied by Tcl
void TclStarIfc::setOneOutput (int outNum, double outValue) {
	if(outNum >= arraySize) {
	    Error::warn(*myStar,"Too many outputs supplied by Tcl");
	} else {
	    outputValues[outNum] = outValue;
	}
}


// Initialize the static counter for unique names.
// The initialization occurs at load time.
int TclStarIfc::unique = 0;
