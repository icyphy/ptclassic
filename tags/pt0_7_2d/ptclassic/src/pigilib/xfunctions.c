/* 
SCCS Version identification :
@(#)xfunctions.c	2.26	04/26/98

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

 Programmer: Joe Buck
 Created: 4/10/90

X window interface routines for Ptolemy.
Converted to Tcl/Tk by Kennard White and Edward Lee.
*/

/* Standard includes */
#include "local.h"		/* include sol2compat.h, ansi.h, compat.h */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>		/* Pick up getpid() */
#include "tcl.h"		/* define Tcl_XXX routines */

/* Pigilib includes */
#include "xfunctions.h"
#include "ptk.h"
#include "vemInterface.h"
#include "err.h"

/*******************************************************************

			win_msg

********************************************************************

  This function pops a tk message window with a multi line message.
  It grabs the focus until the user responds with either a return or
  a click of the OK button.
*/

void
win_msg(omsg)
const char* omsg;
{
	int numChars;
	int flags;
	char *dst;

	/* The message in omsg is a C string.  Need to convert it to */
	/* a Tcl string, adding braces and backslashes if necessary. */
	/* omsg is not altered, but the Tcl prototypes want char*    */
	numChars = Tcl_ScanElement((char *) omsg, &flags) + 1;
	dst = (char *) malloc(numChars);
	Tcl_ConvertElement((char *) omsg, dst, flags);

	if (Tcl_VarEval(ptkInterp, "ptkImportantMessage .ptkMessage ",
			dst, NULL) != TCL_OK) {
		puts(omsg);
		fflush(stdout);
	}
	free(dst);
}

/*******************************************************************

			accum_string

********************************************************************
  This function accumulates strings passed to it in the global
  tcl string "ptkProfileString" for later printing using pr_accum_string.
*/

void
accum_string(string)
const char *string;
{
    Tcl_SetVar(ptkInterp, "ptkProfileString", (char *)string,
	       TCL_APPEND_VALUE | TCL_GLOBAL_ONLY);
}


/*******************************************************************

			clr_accum_string

********************************************************************
  This function clears the accumulated string "profile".
*/
void
clr_accum_string()
{
    Tcl_SetVar(ptkInterp,"ptkProfileString","",TCL_GLOBAL_ONLY);
}

/*******************************************************************

			pr_accum_string

********************************************************************
  This function prints the accumulated string "profile".
  Using tk, the profile is displayed in a window that persists
  until the user destroys it.
*/
void
pr_accum_string()
{
    Tcl_Eval(ptkInterp, "ptkDisplayProfile");
    clr_accum_string();
}

/*******************************************************************

			PrintVersion

********************************************************************

   Pop up the opening message and version identification
*/

extern char *gVersion;		/* defined by pigiRpc (version.c) */
char *pigiFilename = NULL;      /* initialized by pigiMain */

void PrintVersion ()
{
    char buf[256];

    PrintCon(gVersion);
    if ( pigiFilename == NULL )
        pigiFilename = "pigiRpc";
    sprintf(buf, "Running %s, %d", pigiFilename, (int)getpid());
    PrintCon(buf);

    /* If ptkStartupMessage fails, then print an error message.
     * Don't rely on Tycho being there, so don't expect to bring up
     * a stack backtrace
     */
    if (Tcl_VarEval(ptkInterp, "ptkStartupMessage {", gVersion, "} {",
		    pigiFilename, "}", NULL) != TCL_OK) {
        ErrAdd("Unable to bring up ptkStartupMessage.\n");
	ErrAdd("If you are running a debugger, make sure that $PTOLEMY is");
	ErrAdd("properly set in your .cshrc.\n");
	ErrAdd("Also, be sure that $PTOLEMY/tycho/typt/kernel exists.\n");
	ErrAdd(Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
	PrintErr(ErrGet());
	exit(1);
    } 
    /* Link the version and file name with tcl variables so that
     * we can query them from tcl.
     */  
   TCL_CATCH_ERR(Tcl_LinkVar(ptkInterp,
            strdup("pigi_version"),
            (char *) &gVersion,
            TCL_LINK_STRING|TCL_LINK_READ_ONLY));
   TCL_CATCH_ERR(Tcl_LinkVar(ptkInterp,
            strdup("pigi_filename"),
            (char *) &pigiFilename,
            TCL_LINK_STRING|TCL_LINK_READ_ONLY));
}
