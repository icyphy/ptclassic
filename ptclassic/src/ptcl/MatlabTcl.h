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
		       
 Programmer:  B. L. Evans
 Date of creation: 11/16/95

This file implements a class that adds Matlab-specific Tcl commands to
a Tcl interpreter.

**************************************************************************/

#ifndef _MatlabTcl_h
#define _MatlabTcl_h 1

#ifdef __GNUG__
#pragma interface
#endif


#include "tcl.h"
#include "MatlabIfc.h"
#include "DataStruct.h"
#include "StringList.h"
#include "InstanceManager.h"


class MatlabTcl {

public:
	// constructor
	MatlabTcl();

	// destructor
	~MatlabTcl();

	// primary entry point
	int matlab(int argc, char** argv);

	// methods to implement matlab options
	int end(int argc, char** argv);
	int eval(int argc, char** argv);
	int get(int argc, char** argv);
	int getpairs(int argc, char** argv);
	int send(int argc, char** argv);
	int set(int argc, char** argv);
	int start(int argc, char** argv);
	int status(int argc, char** argv);
	int unset(int argc, char** argv);

	// set data members
	Tcl_Interp* SetTclInterpreter(Tcl_Interp* interp);

	// get data members
	Tcl_Interp* GetTclInterpreter();

protected:
	// display usage information in the Tcl interpreter
	int usage(const char* msg);

	// display an error message in the Tcl interpreter
	int error(const char* msg);

	// set Matlab figure handle
	void sethandle();

	// start a Matlab process if one is not running
	int init(char* command = 0);

	// evaluate a Matlab command
	int evaluate(char* command, int outputBufferFlag);

	// the current Tcl interpeter
	Tcl_Interp* tclinterp;

	// the interface to the Matlab engine
	MatlabIfc* matlabInterface;

	// object to control the Tcl/Matlab interface
	InstanceManager manager;
};

#endif
