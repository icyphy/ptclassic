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
 Date of creation: 11/17/95

This file implements a class that adds Ptolemy-specific Tcl commands to
a Tcl interpreter.  Commands are designed to resemble those of the earlier
interpreter.

**************************************************************************/
static const char file_id[] = "MatlabTcl.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include "tcl.h"
#include "Error.h"
#include "MatlabIfc.h"
#include "MatlabIfcFuns.h"
#include "StringList.h"
#include "MatlabTcl.h"

// constructor
MatlabTcl::MatlabTcl() { tclinterp = 0; }

// destructor
MatlabTcl::~MatlabTcl() {}

// set the Tcl interpreter held in the interp data member
Tcl_Interp* MatlabTcl::SetTclInterpreter(Tcl_Interp* interp) {
    tclinterp = interp;
    return tclinterp;
}

// get the Tcl interpreter held in the interp data member
Tcl_Interp* MatlabTcl::GetTclInterpreter() {
    return tclinterp;
}

// display usage message in Tcl
int MatlabTcl::usage(const char* msg) {
    Tcl_AppendResult(tclinterp, "incorrect usage: should be \"", msg, "\"", 0);
    return TCL_ERROR;
}

// display error message in Tcl
int MatlabTcl::error(const char* msg) {
    Tcl_AppendResult(tclinterp, "error: ", msg, 0);
    return TCL_ERROR;
}

// evaluate the Tcl command "matlab"
int MatlabTcl::matlab(int argc, char** argv) {

    if ( tclinterp == 0 ) {
	Error::error("Tcl interpreter not initialized");
	return TCL_ERROR;
    }

    if ( argc == 1 ) {
        return error("wrong # args in \"matlab\" command");
    }

    if ( strcmp(argv[1], "end") == 0 ) {
	return MatlabTcl::end(argc, argv);
    }
    else if ( strcmp(argv[1], "eval") == 0 ) {
	return MatlabTcl::eval(argc, argv);
    }
    else if ( strcmp(argv[1], "get") == 0 ) {
	return MatlabTcl::get(argc, argv);
    }
    else if ( strcmp(argv[1], "send") == 0 ) {
	return MatlabTcl::send(argc, argv);
    }
    else if ( strcmp(argv[1], "set") == 0 ) {
	return MatlabTcl::set(argc, argv);
    }
    else if ( strcmp(argv[1], "start") == 0 ) {
	return MatlabTcl::start(argc, argv);
    }
    else if ( strcmp(argv[1], "unset") == 0 ) {
	return MatlabTcl::unset(argc, argv);
    }
    else {
	StringList msg = "unrecognized matlab command \"";
	msg << argv[1]
	    << "\": should be end, eval, get, send, set, start, or unset";
	return error(msg);
    }

    return TCL_OK;
}

// close a Matlab connection: don't do anything except close figures
int MatlabTcl::end(int argc, char** /*argv*/) {
    if (argc != 2) return usage("matlab end");
    matlabInterface.CloseMatlabFigures();
    return TCL_OK;
}

// evaluate a Matlab command
int MatlabTcl::eval(int argc, char** argv) {
    if (argc != 3) return usage("matlab eval <matlab_command>");
    matlabInterface.AttachMatlabFigureIds();
    int merror = matlabInterface.EvaluateOneCommand(argv[2]);
    Tcl_AppendResult(tclinterp, matlabInterface.GetOutputBuffer(), 0);
    if ( merror ) return TCL_ERROR;
    else return TCL_OK;
}

// get a Matlab matrix
int MatlabTcl::get(int argc, char** /*argv*/) {
    if (argc != 3) return usage("matlab get <matrix_name>");
    return TCL_OK;
}

// evaluate a Matlab command
int MatlabTcl::send(int argc, char** argv) {
    if (argc != 3) return usage("matlab send <matlab_command>");
    matlabInterface.AttachMatlabFigureIds();
    int merror = matlabInterface.EvaluateOneCommand(argv[2]);
    if ( merror ) return TCL_ERROR;
    return TCL_OK;
}

// set a Matlab matrix
int MatlabTcl::set(int argc, char** argv) {
    char* usagestr = "matlab set <var> <numrows> <numcols> <real_components> ?<imag_components>?";

    if (argc < 6 || argc > 7) return usage(usagestr);
    int numrows, numcols;
    if ( sscanf(argv[3], "%d", &numrows) != 1 ) return usage(usagestr);
    if ( sscanf(argv[4], "%d", &numcols) != 1 ) return usage(usagestr);

    Matrix* matrix = 0;
    if ( argc == 6 ) {
/* Figure out what to do here: we want to parse argv[5] and argv[6]
   as lists of numbers */
/*
       matrix = matlabInterface.SetVariable(argv[2], numrows, numcols,
					    argv[5], 0);
 */
    }
    else {
/*
       matrix = matlabInterface.SetVariable(argv[2], numrows, numcols,
					    argv[5], arg[6]);
 */
    }
    if ( matrix ) return TCL_OK;
    else return TCL_ERROR;
}

// start a Matlab process if one is not running already
int MatlabTcl::start(int argc, char** /*argv*/) {
    if (argc != 2) return usage("matlab start");
    if (! matlabInterface.MatlabIsRunning()) {
	if (! matlabInterface.StartMatlab() ) {
	    return error("cannot start matlab");
	}
    }
    return TCL_OK;
}

// unset a Matlab matrix
int MatlabTcl::unset(int argc, char** /*argv*/) {
    if (argc != 3) return usage("matlab unset <matrix_name>");
    return TCL_OK;
}
