/*******************************************************************
Define a console window object.  This object has a Tcl interpreter
associated with it, created by the constructor.

$Id$
Programmer: E. A. Lee

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

********************************************************************/

#ifndef _TyConsole_h
#define _TyConsole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "ptk.h"
#include "PTcl.h"
#include "TyTcl.h"
#include "SimAction.h"

class TyConsole {

 public:

  // Constructor and destructor
  // The constructor sets up the Tcl interpreter and Tk main window.
  // Failures in the constructor are fatal, halting the application.
  // The command line arguments should be passed to the constructor
  // so that options can be passed.
  TyConsole(int argc, char **argv);
  ~TyConsole();

  // Access the tcl interpreter, the ptcl object (supplying the Ptolemy
  // extensions to tcl) and the tk window.
  Tcl_Interp* tclInterp();
  PTcl* ptclInterp();
  Tk_Window* tkWindow();

  // Call this to exit the program cleanly.
  // It should never return.
  void tyExit(int code);

protected:
  // the Tcl interpreter for this console
  Tcl_Interp *interp;

  // the Ptolemy interpreter for this console
  PTcl *ptcl;

  // the object containing the Tycho extensions to Tcl
  TyTcl *tytcl;

  // the top-level tk window for this console
  Tk_Window ptkw;

  // Initialize Tcl and Tk and register the locally defined Tcl commands
  // with the Tcl interpreter. Returns TCL_ERROR or TCL_OK.
  int appInit(Tcl_Interp *ip, Tk_Window win);
};

#endif
