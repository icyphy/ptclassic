static const char file_id[] = "ptclInvokeInterp.cc";
/*******************************************************************
SCCS Version identification :
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

 Programmer: B. L. Evans
 Date of creation: 9/26/95

Implementation of the InvokeInterp class that calls the Tcl interpreter.

*******************************************************************/

#include "PTcl.h"
#include "Error.h"
#include "InvokeInterp.h"
#include "StringList.h"

const char*
InvokeInterp :: interpreter (const char* expression) {
  const char* result = 0;
  // If there is no active PTcl interpreter, report an error
  if ( PTcl::activeInterp == 0 ) {
    Error::error("The Tcl interpreter is not initialized");
  }
  else {
    if ( Tcl_Eval(PTcl::activeInterp, expression) == TCL_OK ) {
      result = PTcl::activeInterp->result;
    }
    else {
      StringList msg;
      msg << "The Tcl interpreter reported an error in evaluating '"
	  << expression
	  << "': "
	  << PTcl::activeInterp->result;
      Error::error(msg);
    }
  }
  return result;
}
