static const char file_id[] = "displayFile.c";

/* 
Functions that display a file

Version identification:
$Id$

Copyright (c) 1996-1997 The Regents of the University of California.
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
Author: Christopher Hylands 9/3/96
*/

#include "displayFile.h"
#include <tcl.h>
#include <string.h>
#include <stdlib.h>		/* For system and getenv */

/* We could pass this around everywhere and have displayFile.h include
 * tcl.h, but that would add a layer of complication.
 */
Tcl_Interp *ptkInterp;

/* The following provides more convenient and consistent access to Tcl
   with error checking.  See pigilib/err.h  */
#define TCL_CATCH_ERR(tcl_call) \
    if ( (tcl_call) != TCL_OK) { \
	Tcl_Eval(ptkInterp,"ptkDisplayErrorInfo"); \
    }
/*
 * Generate an operating system command to display text files.
 * If the PT_DISPLAY environment variable is not set, then set
 * buf to the empty string.
 * If the PT_DISPLAY environment variable is set, then print the
 * value of $PT_DISPLAY into buf and put the job in the background.
 * PT_DISPLAY should be a printf format string, such as
 * "xedit -name ptolemy_code %s".
 */
static void genDispCommand(char *buf, const char *fileName)
{
    char* dispCmd = getenv("PT_DISPLAY");
    if (dispCmd == 0) {
	*buf = '\0';
    }
    else {
	sprintf(buf, dispCmd, fileName);
	strcat(buf, " &");
    }
}

/*
 * Start tycho if it is not already running.
 */
void startTycho() {
    TCL_CATCH_ERR( Tcl_VarEval(ptkInterp, "ptkStartTycho", (char*)NULL));
}

/*
 * Display a file 
 * We must do the right thing in the following situations:
 * If ptcl is running, then we should check the DISPLAY variable, and
 * if it is set, exec tycho <filename>.
 * If we are inside pigi, we may need to start tycho.
 *  
 * Currently, this function is called from pigilib/icon.c and from
 * cg/kernel/CGUtilties.cc
 *
 * fileName	- The name of the file to open
 * debugFuncPtr - A function that takes a const char * that will print
 *		  out status information about what is going on.  This
 *		  is useful if there is a slight delay in getting things
 *		  started
 * errFuncPtr	- A functino that takes a const char * that will print
 *		  an error message if something goes wrong.
 * If debugFuncPtr or errFuncPtr are NULL, then they are not called.
 */	
int displayFile(const char *fileName,
		void (*debugFuncPtr)(const char *),
		void (*errFuncPtr)(const char *))
{
  char buf[512];
  genDispCommand(buf, fileName);
  if (*buf == '\0') {
    if (debugFuncPtr != (void (*)(const char *))NULL )    
      (debugFuncPtr)("Invoking Tycho editor");
    startTycho();
    if (( Tcl_VarEval(ptkInterp,
		      "::tycho::File::openContext ",
		      fileName,
		      (char *)NULL) ) != TCL_OK) {
      sprintf(buf, "Cannot invoke Tycho editor for '%s':\n%s", fileName,
	      Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
      if (errFuncPtr != (void (*)(const char *)) NULL) 
	(errFuncPtr)(buf);
      return 0;
    }
    return 1;
  }
  else {
    if (debugFuncPtr != (void (*)(const char *)) NULL)    
      (debugFuncPtr)(buf);
    if (system(buf)) {
      sprintf(buf, "Cannot edit Ptolemy code file '%s'", fileName);
      if (errFuncPtr != (void (*)(const char *)) NULL)
	(errFuncPtr)(buf);
      return 0;
    }
    return 1;
  }
}
