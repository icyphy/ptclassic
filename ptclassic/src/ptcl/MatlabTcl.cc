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

This file implements a class that adds Matlab-specific Tcl commands to
a Tcl interpreter.

**************************************************************************/
static const char file_id[] = "MatlabTcl.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include <tcl.h>
#include "miscFuncs.h"
#include "StringList.h"
#include "Error.h"
#include "MatlabIfcFuns.h"
#include "MatlabIfc.h"
#include "MatlabTcl.h"

#define MATLABTCL_CHECK_MATLAB() \
	if (! init()) return error("Could not start MATLAB")

// constructor
MatlabTcl::MatlabTcl() {
    tclinterp = 0;
    matlabInterface = 0;
}

// destructor
MatlabTcl::~MatlabTcl() {
    delete matlabInterface;
}

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

// convert the Tcl interpreter pointer to a unique string identifier
void MatlabTcl::sethandle() {
    char handle[32];
    sprintf(handle, "MatlabTcl%-.8lx", long(tclinterp));
    matlabInterface->SetFigureHandle(handle);
}

// start a Matlab process if one is not running already
int MatlabTcl::init(char* command) {
    if (matlabInterface == 0) {
	matlabInterface = new MatlabIfc;
	matlabInterface->SetDeleteFigures(TRUE);
    }
    if (! matlabInterface->MatlabIsRunning()) {
	if (! matlabInterface->StartMatlab(command) ) {
	    return FALSE;
	}
    }
    return TRUE;
}

// support evaluation of a Matlab command
int MatlabTcl::evaluate(char* command, int outputBufferFlag) {
    sethandle();
    int merror = matlabInterface->EvaluateUserCommand(command);
    if (outputBufferFlag || merror) {
	Tcl_AppendResult(tclinterp, matlabInterface->GetOutputBuffer(), 0);
    }
    if (merror) return TCL_ERROR;
    return TCL_OK;
}

// evaluate the Tcl command "matlab"
int MatlabTcl::matlab(int argc, char** argv) {

    if (tclinterp == 0) {
	Error::error("Tcl interpreter not initialized");
	return TCL_ERROR;
    }

    if (argc == 1) {
        return error("wrong # args in \"matlab\" command");
    }

    // switch on the first letter of the command (second argument)
    switch (*argv[1]) {
      case 'e':
	if ( strcmp(argv[1], "end") == 0 ) {
	    return MatlabTcl::end(argc, argv);
	}
	else if ( strcmp(argv[1], "eval") == 0 ) {
	    return MatlabTcl::eval(argc, argv);
	}
	break;

      case 'g':
	if ( strcmp(argv[1], "get") == 0 ) {
	    return MatlabTcl::get(argc, argv);
	}
	else if ( strcmp(argv[1], "getpairs") == 0 ) {
	    return MatlabTcl::getpairs(argc, argv);
	}
	break;

      case 's':
	if ( strcmp(argv[1], "send") == 0 ) {
	    return MatlabTcl::send(argc, argv);
	}
	else if ( strcmp(argv[1], "set") == 0 ) {
	    return MatlabTcl::set(argc, argv);
	}
	else if ( strcmp(argv[1], "start") == 0 ) {
	    return MatlabTcl::start(argc, argv);
	}
	else if ( strcmp(argv[1], "status") == 0 ) {
	    return MatlabTcl::status(argc, argv);
	}
	break;

      case 'u':
	if ( strcmp(argv[1], "unset") == 0 ) {
	    return MatlabTcl::unset(argc, argv);
	}
	break;
    }

    StringList msg = "unrecognized matlab command \"";
    msg << argv[1]
	<< "\": should be "
	<< "end, eval, get, getpairs, send, set, start, status, or unset";
    return error(msg);
}

// close a Matlab connection: don't terminate the Matlab connection,
// but close figures generated by this instance
int MatlabTcl::end(int argc, char** argv) {
    if (argc < 2 || argc > 3) return usage("matlab end ?<identifier>?");
    if (matlabInterface == 0) {
	return error("the Tcl/Matlab interface has not been initialized");
    }
    char* id = (argc == 3) ? argv[2] : 0;
    Pointer key = manager.makeInstanceName(tclinterp, id);
    if (! manager.exists(key) ) {
	StringList msg = "the Tcl/Matlab interface has not been initialized";
	if (id) msg << " for " << id;
	return error(msg);
    }
    manager.remove(key);
    if ( manager.noMoreInstances() ) {
	matlabInterface->CloseMatlabFigures();
	delete matlabInterface;
	matlabInterface = 0;
    }
    return TCL_OK;
}

// evaluate a Matlab command
int MatlabTcl::eval(int argc, char** argv) {
    if (argc != 3) return usage("matlab eval <matlab_command>");
    MATLABTCL_CHECK_MATLAB();
    return evaluate(argv[2], TRUE);
}

// get a Matlab matrix
int MatlabTcl::get(int argc, char** argv) {
    if ((argc < 3) || (argc > 4))
	return usage("matlab get <matrix_name> ?<matlab_command>?");
    MATLABTCL_CHECK_MATLAB();

    // evaluate the script if given
    if (argc == 4) {
	if ( evaluate(argv[3], FALSE) != TCL_OK ) return TCL_ERROR;
    }

    int numrows = 0, numcols = 0;
    char** realStrings = 0;
    char** imagStrings = 0;

    // return a four-element list: numrows numcols realvalues imagvalues
    if ( matlabInterface->GetMatlabVariable(argv[2], &numrows, &numcols,
					    &realStrings, &imagStrings) ) {
	char tmpbuf[64];
	sprintf(tmpbuf, "%d", numrows);
	Tcl_AppendElement(tclinterp, tmpbuf);
	sprintf(tmpbuf, "%d", numcols);
	Tcl_AppendElement(tclinterp, tmpbuf);
	int numelements = numrows * numcols;
        char* realList = Tcl_Merge(numelements, realStrings);
	Tcl_AppendElement(tclinterp, realList);
	free(realList);
	matlabInterface->FreeStringArray(realStrings, numelements);
	delete [] realStrings;
	if (imagStrings) {
	    char* imagList = Tcl_Merge(numelements, imagStrings);
	    Tcl_AppendElement(tclinterp, imagList);
	    free(imagList);
	    matlabInterface->FreeStringArray(imagStrings, numelements);
	    delete [] imagStrings;
	}
	return TCL_OK;
    }

    StringList msg = "Matlab matrix '";
    msg << argv[2] << "' is not defined";
    return error(msg);
}

// get a Matlab matrix
int MatlabTcl::getpairs(int argc, char** argv) {
    if ((argc < 3) || (argc > 4))
	return usage("matlab getpairs <matrix_name> ?<matlab_command>?");
    MATLABTCL_CHECK_MATLAB();

    // evaluate the script if given
    if (argc == 4) {
	if ( evaluate(argv[3], FALSE) != TCL_OK ) return TCL_ERROR;
    }

    InfString command = "eval makeOrderedPairs [lrange [matlab get ";
    command << argv[2] << "] 2 end]";
    return Tcl_Eval(tclinterp, command);
}

// evaluate a Matlab command
int MatlabTcl::send(int argc, char** argv) {
    if (argc != 3) return usage("matlab send <matlab_command>");
    MATLABTCL_CHECK_MATLAB();
    return evaluate(argv[2], FALSE);
}

// set a Matlab matrix
int MatlabTcl::set(int argc, char** argv) {
    char* usagestr = "matlab set <var> <numrows> <numcols> <real_components> ?<imag_components>?";

    // check for the right number of arguments
    if (argc < 6 || argc > 7) return usage(usagestr);

    // check to see if Matlab is available
    MATLABTCL_CHECK_MATLAB();

    // parse the values given for the number of rows and number of columns
    int numrows = 0, numcols = 0;
    if ( Tcl_GetInt(tclinterp, argv[3], &numrows) != TCL_OK || numrows <= 0 ) {
	return usage(usagestr);
    }
    if ( Tcl_GetInt(tclinterp, argv[4], &numcols) != TCL_OK || numcols <= 0 ) {
	return usage(usagestr);
    }

    // parse the values given for the real and imaginary values
    int numelements = numrows * numcols;
    int realArgc = 0;
    char **realArgv = 0;
    int imagArgc = 0;
    char **imagArgv = 0;
    if (Tcl_SplitList(tclinterp, argv[5], &realArgc, &realArgv) != TCL_OK) {
	return TCL_ERROR;
    }
    if (realArgc != numelements) {
	return error("matlab set: not enough real-valued elements given");
    }
    if ( argc == 7 ) {
	if (Tcl_SplitList(tclinterp, argv[6], &imagArgc, &imagArgv) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (imagArgc != numelements) {
	    return error("matlab set: not enough imaginary elements given");
	}
    }

    // set the Matlab matrix whose name is the third argument, argv[2]
    if ( matlabInterface->SetMatlabVariable(argv[2], numrows, numcols,
					    realArgv, imagArgv) ) {
	return TCL_OK;
    }
    else {
	return error("matlab set: Matlab could not allocate enough memory");
    }
}

// start a Matlab process if one is not running already
int MatlabTcl::start(int argc, char** argv) {
    if (argc < 2 || argc > 4) 
        return usage("matlab start ?<identifier>? ?<start_command>?");
    char* startCommand = (argc == 4) ? argv[3] : 0;
    if (! init(startCommand)) return error("Could not start matlab");
    char* id = (argc == 3) ? argv[2] : 0;
    manager.add(tclinterp, id);
    return TCL_OK;
}

// return the status of the Matlab-Tcl interface
int MatlabTcl::status(int argc, char** /*argv*/) {
    if (argc != 2) return usage("matlab status");
    if (matlabInterface == 0) {
	Tcl_SetResult(tclinterp, "-1", TCL_STATIC);
    } 
    else if (matlabInterface->MatlabIsRunning()) {
	Tcl_SetResult(tclinterp, "0", TCL_STATIC);
    }
    else {
	Tcl_SetResult(tclinterp, "1", TCL_STATIC);
    }
    return TCL_OK;
}

// unset a Matlab matrix
int MatlabTcl::unset(int argc, char** argv) {
    if (argc != 3) return usage("matlab unset <matrix_name>");
    MATLABTCL_CHECK_MATLAB();
    int merror = matlabInterface->UnsetMatlabVariable(argv[2]);
    if (merror) return TCL_ERROR;
    return TCL_OK;
}
