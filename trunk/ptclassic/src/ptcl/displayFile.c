static const char file_id[] = "DisplayFile.cc";
/* 
Functions that Display a file

Version identification:
$Id$

Copyright (c) 1996 The Regents of the University of California.
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

#include "DisplayFile.c"
#include <tcl.h>

/*
 * If the PT_DISPLAY environment variable is not set, then set
 * buf to the empty string.
 * If the PT_DISPLAY environment variable is set, then use print
 * the value of $PT_DISPLAY into buf.  PT_DISPLAY should 
 * be printf format string, like "xedit -name ptolemy_code %s".
 */
static void genDispCommand(char *buf, const char *fileName, int background)
{
    char* dispCmd = getenv("PT_DISPLAY");
    if (dispCmd == 0) {
	*buf = '\0';
    } else {
	sprintf(buf, dispCmd, file);
	if (background) strcat(buf, "&");
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
 */
int DisplayFile( const char *fileName, void (*errFuncPtr)(char *))
{
  char buf[512];
  genDispCommand(buf, fileName, TRUE);
  if (*buf == '\0') {
    PrintDebug("Invoking Tycho editor");
    startTycho();
    if (( Tcl_VarEval(ptkInterp,
		      "::tycho::File::openContext ",
		      fileName,
		      (char *)NULL) ) != TCL_OK) {
      sprintf(buf, "Cannot invoke Tycho editor for '%s'", fileName);
      *errFuncPtr(buf);
      return (FALSE);
    }
    return (TRUE);
  } else {
    PrintDebug(buf);
    if (system(buf)) {
      sprintf(buf, "Cannot edit Ptolemy code file '%s'", fileName);
      *errFuncPtr(buf);
      return (FALSE);
    }
    return (TRUE);
  }

}

