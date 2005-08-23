/*
Copyright (c) 1990-1997 The Regents of the University of California.
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

  Author:  Brian L. Evans and Steve X. Gu
  Created: 01/13/96
  Version: @(#)MathematicaTcl.h	1.6	08/17/97

  This file implements a class that adds Mathematica-specific Tcl commands
  to a Tcl interpreter.
 */

#ifndef _MathematicaTcl_h
#define _MathematicaTcl_h 

#ifdef __GNUG__
#pragma interface
#endif

#include <tcl.h>
#include "StringList.h"
#include "MathematicaIfc.h"
#include "InstanceManager.h"

class MathematicaTcl {

public:
    // Constructor
    MathematicaTcl();

    // Destructor
    ~MathematicaTcl();    

    // primary entry point
    int mathematica(int argc, char** argv);

    // Methods to implement mathematica options
    int end(int argc, char** argv);
    int eval(int argc, char** argv);
    int get(int argc, char** argv);
    int send(int argc, char** argv);
    int start(int argc, char** argv);
    int status(int argc, char** argv);

    // Set Tcl interpreter
    Tcl_Interp* SetTclInterpreter(Tcl_Interp* interp);

    // Get Tcl interpreter
    Tcl_Interp* GetTclInterpreter();

protected:
    // Display usage information in the Tcl interpreter
    int usage(const char* msg);

    // Display an error message in the Tcl interpreter
    int error(const char* msg);

    // Create a new interface to Mathematica
    void newMathematicaInterface();

    // Start a Mathematica process if one is not running
    int init();

    // Evaluate a Mathematica command
    int evaluate(char* command, int outputBufferFlag);

    // The current Tcl interpreter
    Tcl_Interp* tclinterp;

    // The interface to the Mathematica kernel
    MathematicaIfc* mathematicaInterface;

    // object to control the Tcl/Mathematica interface
    InstanceManager manager;
};

#endif
