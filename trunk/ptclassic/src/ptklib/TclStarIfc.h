#ifndef _TclStarIfc_h
#define _TclStarIfc_h 1
/**************************************************************************
Version identification:
$Id$

Author: Edward A. Lee

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

#ifdef __GNUG__
#pragma interface
#endif

#include "Block.h"
#include "StringList.h"
extern "C" {
#include "ptk.h"
}

// TclStarIfc will be instantiated by each star 

class TclStarIfc {
public:
	// constructor and destructor
	TclStarIfc ();
	~TclStarIfc ();

	// Define the relevant Tcl variables and functions, and then source
	// the specified Tcl file.  All the star's parameters are made available
	// to Tcl.
	// Returns FALSE if setup fails, TRUE otherwise.
	// The numInputs and numOutputs arguments are made available to Tcl,
	// and also determine whether the grabInputs and setOutputs procedures
	// are defined. The value of numInputs should equal the total number
	// of input ports that the star has.  The value of numOutputs can
	// can be any non-negative integer.  Any call to "setOutputs" from
	// tcl must have this number of arguments.  Some stars use these
	// values to set the outputs of the star, in which case the number
	// should match the number of outputs.
	int setup (Block* star, int numInputs,
		   int numOutputs, const char* tcl_file);

	// Invoke the callTcl procedure, if it was defined in the Tcl script.
	// Return FALSE if an error occurs, TRUE otherwise.
	int go ();

	// Method to get inputs from myStar
	StringList getInputs();

	// Array of values that can be set from Tcl using the "setOutputs"
	// command.
	double *outputValues;

	// Method to set one value of the above array with boundary checking
	void setOneOutput(int outNum, double outValue);

	Block* myStar;
protected:
	// A static variable used to create names that are
	// guaranteed to be unique.  After each use, it should
	// be incremented.
	static unique;

	// Size of the "outputValues" array.
	int arraySize;

	// A unique string for each instance of this object identifies
	// the star within which it sits for the benefit of Tcl routines.
	StringList starID;

	// A flag specifying whether a callTcl procedure was defined
	// when the Tcl file was sourced.
	int synchronous;

};
#endif
