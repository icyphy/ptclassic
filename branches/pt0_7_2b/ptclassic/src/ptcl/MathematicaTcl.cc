/*
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

  Programmer: Steve X. Gu and Brian L. Evans
  Date of creation: 01/13/96

  This file implements a class that adds Mathematica-specific Tcl commands
  to a Tcl interpreter.
 */

static const char file_id[] = "MathematicaTcl.cc";

#ifndef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include "tcl.h"
#include "StringList.h"
#include "Error.h"
#include "MathematicaIfc.h"
#include "MathematicaTcl.h"

#define MATHEMATICATCL_CHECK_MATHEMATICA() \
        if (! init()) return error("Could not start Mathematica")

// Consturctor
MathematicaTcl::MathematicaTcl() {
    tclinterp = 0;
    mathematicaInterface = 0;
}

// Destructor
MathematicaTcl::~MathematicaTcl() {
    delete mathematicaInterface;
}    

// Set the Tcl interpreter held in the interp data member
Tcl_Interp* MathematicaTcl::SetTclInterpreter(Tcl_Interp* interp) {
    tclinterp = interp;
    return tclinterp;
}

// Get the Tcl interpreter held in the interp data member
Tcl_Interp* MathematicaTcl::GetTclInterpreter() {
    return tclinterp;
}

// Display usage information in the Tcl interpreter
int MathematicaTcl::usage(const char* msg) {
    Tcl_AppendResult(tclinterp, "incorrect usage: should be \"", msg, "\"", 0);
    return TCL_ERROR;
}

// Display an error message in the Tcl interpreter
int MathematicaTcl::error(const char* msg) {
    Tcl_AppendResult(tclinterp, "error: ", msg, 0);
    return TCL_ERROR;
}

// Start a Mathematica process if one is not running
int MathematicaTcl::init() {
    if (mathematicaInterface == 0) {
	mathematicaInterface = new MathematicaIfc;
    }

    if (! mathematicaInterface->MathematicaIsRunning()) {
	if (! mathematicaInterface->StartMathematica() ) {
	    return FALSE;
	}
    }

    return TRUE;
}

// Evaluate a Mathematica command
int MathematicaTcl::evaluate(char* command, int outputBufferFlag) {
    int merror = mathematicaInterface->EvaluateOneCommand(command);
    if (outputBufferFlag || merror) {
	Tcl_AppendResult(tclinterp, 
			 mathematicaInterface->GetOutputBuffer(), 
			 0);
    }

    if (merror) return TCL_ERROR;
    return TCL_OK;
}

// Evaluate the Tcl command "mathematica"
int MathematicaTcl::mathematica(int argc, char** argv)
{
    if (tclinterp == 0) {
	Error::error("Tcl interpreter not initialized");
	return TCL_ERROR;
    }

    if (argc == 1) {
	return error("wrong # args in \"mathematica\" command");
    }

    // Switch on the first letter of the command (second argument)
    switch (*argv[1]) {

      case 'e':
	if ( strcmp(argv[1], "end") == 0 ) {
	    return MathematicaTcl::end(argc, argv);
	}
	else if ( strcmp(argv[1], "eval") == 0 ) {
	    return MathematicaTcl::eval(argc, argv);	    
	}
	break;
	
      case 's':
	if ( strcmp(argv[1], "send") == 0 ) {
	    return MathematicaTcl::send(argc, argv);
	}
	else if ( strcmp(argv[1], "start") == 0 ) {
	    return MathematicaTcl::start(argc, argv);
	}
	else if ( strcmp(argv[1], "status") == 0 ) {
	    return MathematicaTcl::status(argc, argv);
	}
	break;

      // FIXME: Add more cases to support more commands. 
    }

    StringList msg = "unrecognized mathematica command \"";
    msg << argv[1]
	<< "\", should be "
	<< "end, eval, send, start, or status";

    return error(msg);
}

// Close a Mathematica connection
int MathematicaTcl::end(int argc, char** /*argv*/) {
    if (argc != 2) return usage("mathematica end");
    if (mathematicaInterface == 0) {
	return error("the Tcl/Mathematica interface has not been initialized");
    }
    delete mathematicaInterface;
    mathematicaInterface = 0;
    return TCL_OK;
}

// Evaluate a Mathematica command
int MathematicaTcl::eval(int argc, char** argv) {
    if (argc != 3) return usage("mathematica eval <mathematica_commad>");
    MATHEMATICATCL_CHECK_MATHEMATICA();
    return evaluate(argv[2], TRUE);
}

// Evaluate a Mathematica command
int MathematicaTcl::send(int argc, char** argv) {
    if (argc != 3) return usage("mathematica send <mathematica_commad>");
    MATHEMATICATCL_CHECK_MATHEMATICA();
    return evaluate(argv[2], FALSE);
}

// Start a Mathematica process if one is not running already
int MathematicaTcl::start(int argc, char** /*argv*/){
    if (argc != 2) return usage("mathematica start");
    if (init()) return TCL_OK;
    return error("Could not start mathematica");
}

// Return the status of the Tcl/Mathematica interface
int MathematicaTcl::status(int argc, char** /*argv*/) {
    if (argc != 2) return usage("mathematica status");
    if (mathematicaInterface == 0) {
	Tcl_SetResult(tclinterp, "-1", TCL_STATIC);
    }
    else if ( mathematicaInterface->MathematicaIsRunning() ) {
	Tcl_SetResult(tclinterp, "0", TCL_STATIC);	
    }
    else {
	Tcl_SetResult(tclinterp, "1", TCL_STATIC);	
    }
    
    return TCL_OK;
}
