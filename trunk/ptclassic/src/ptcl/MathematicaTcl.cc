/*
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

  Author:  Steve X. Gu and Brian L. Evans
  Created: 01/13/96
  Version: $Id$

  This file implements a class that adds Mathematica-specific Tcl commands
  to a Tcl interpreter.
 */

static const char file_id[] = "MathematicaTcl.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include <tcl.h>
#include "miscFuncs.h"
#include "StringList.h"
#include "InfString.h"
#include "Error.h"
#include "MathematicaIfc.h"
#include "MathematicaTcl.h"

#define MATHEMATICATCL_NOT_START "Could not start Mathematica"

#define MATHEMATICATCL_CHECK_MATHEMATICA() \
        if (! init()) return error(MATHEMATICATCL_NOT_START)

// Constructor
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

// Create a new instance of an interface to Mathematica
void MathematicaTcl::newMathematicaInterface() {
    // 1. put the directory $PTOLEMY/lib/mathematica on Mathematica's path
    StringList initCommand = "PrependTo[$Path, \"";
    char* libMmaName = expandPathName("$PTOLEMY/lib/mathematica");
    initCommand << libMmaName << "\"]; ";

    // 2. declare packages that we need
    initCommand << "Needs[\"FourierSeriesFormula`\"]; ";
    initCommand << "Needs[\"Calculus`Master`\"]; ";
    initCommand << "Needs[\"Algebra`Master`\"]; ";

    // 3. create new instance
    delete mathematicaInterface;
    mathematicaInterface =
    	new MathematicaIfc(initCommand, "MathematicaTcl", TRUE, FALSE, FALSE);

    delete [] libMmaName;
}

// Start a Mathematica process if one is not running
int MathematicaTcl::init() {
    // Start a connection to Mathematica called MathematicaTcl in which
    // Mathematica starts up in a private namespace (context) "MathematicaTcl`"
    // We also disable the display of the input and the output number
    if (mathematicaInterface == 0) {
	newMathematicaInterface();
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
    int retval = mathematicaInterface->EvaluateUserCommand(command);
    if (outputBufferFlag || ! retval) {
	Tcl_AppendResult(tclinterp, 
			 mathematicaInterface->GetOutputBuffer(), 
			 0);
    }

    if (retval) return TCL_OK;
    return TCL_ERROR;
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
	
      case 'g':
	if ( strcmp(argv[1], "get") == 0 ) {
	    return MathematicaTcl::get(argc, argv);
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
int MathematicaTcl::end(int argc, char** argv) {
    if (argc < 2 || argc > 3) return usage("mathematica end ?<identifier>?");
    if (mathematicaInterface == 0) {
	return error("the Tcl/Mathematica interface has not been initialized");
    }
    char* id = (argc == 3) ? argv[2] : 0;
    Pointer key = manager.makeInstanceName(tclinterp, id);
    if (! manager.exists(key) ) {
        StringList msg = "the Tcl/Mathematica interface has not been started";
        msg << " for " << (id ? id : "that instance");
        return error(msg);
    }
    manager.remove(key);
    if ( manager.noMoreInstances() ) {
	mathematicaInterface->CloseMathematicaFigures();
        delete mathematicaInterface;
        mathematicaInterface = 0;
    }
    return TCL_OK;
}

// Evaluate a Mathematica command
int MathematicaTcl::eval(int argc, char** argv) {
    if (argc != 3) return usage("mathematica eval <script>");
    MATHEMATICATCL_CHECK_MATHEMATICA();
    return evaluate(argv[2], TRUE);
}

// Evaluate a Mathematica command, and get a variable
int MathematicaTcl::get(int argc, char** argv) {
    if (argc != 3 && argc != 4) {
	return usage("mathematica get <name> ?<script>?");
    }
    MATHEMATICATCL_CHECK_MATHEMATICA();

    // Evaluate the script given by argv[3]
    int retval = TCL_OK;
    if (argc == 4) {
	retval = evaluate(argv[3], FALSE);
    }

    // Get the value of the Mathematica variable name given by argv[2]
    if (retval == TCL_OK) {
	InfString getTclForm = "InputForm[N[";
	getTclForm << argv[2] << "]]";
	int retval2 =
	    mathematicaInterface->EvaluateUnrecordedUserCommand(getTclForm);
        Tcl_AppendResult(tclinterp,
	    mathematicaInterface->GetPrivateOutputBuffer(),
	    0);
	retval = retval2 ? TCL_OK : TCL_ERROR;
    }
    return retval;
}

// Evaluate a Mathematica command
int MathematicaTcl::send(int argc, char** argv) {
    if (argc != 3) return usage("mathematica send <script>");
    MATHEMATICATCL_CHECK_MATHEMATICA();
    return evaluate(argv[2], FALSE);
}

// Start a Mathematica process if one is not running already
int MathematicaTcl::start(int argc, char** argv){
    if (argc < 2 || argc > 3) return usage("mathematica start ?<identifier>?");
    if (! init()) error(mathematicaInterface->GetErrorString());
    char* id = (argc == 3) ? argv[2] : 0;
    manager.add(tclinterp, id);
    return TCL_OK;
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
