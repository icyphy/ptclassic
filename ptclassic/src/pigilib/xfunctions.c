/* 
Copyright (c) 1990-1993 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/*
 * Tcl/Tk window interface routines for Ptolemy.
 */

#ifndef lint
static char	sccsid_xfunctions[] = "$Id$";
#endif

#include <stdlib.h>
#include "ptk.h"
#include <stdio.h>


/*******************************************************************

			win_msg

********************************************************************

  This function pops a tk message window with a multi line message.
  It grabs the focus until the user responds with either a return or
  a click of the OK button.
*/

void
win_msg(omsg)
char * omsg;
{
	Tcl_CmdBuf buf;
	char* command;
	buf = Tcl_CreateCmdBuf();
	Tcl_AssembleCmd(buf,"ptkImportantMessage .ptkMessage {");
	Tcl_AssembleCmd(buf,omsg);
	command = Tcl_AssembleCmd(buf,"}\n");
	if(Tcl_Eval(ptkInterp, command, 0, (char **) NULL) != TCL_OK) {
		fputs(omsg,stdout);
	}
}

/*******************************************************************

			accum_string

********************************************************************
  This function accumulates strings passed to it in the global
  tcl string "ptkProfileString" for later printing using pr_accum_string.
*/

void
accum_string(string)
	char *string;
{
    Tcl_SetVar(ptkInterp,"ptkProfileString",string,
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
    Tcl_Eval(ptkInterp, "ptkDisplayProfile", 0, (char **) NULL);
    clr_accum_string();
}

/*******************************************************************

			PrintVersion

********************************************************************

   Pop up the opening message and version identification

*/
extern char *pigiVersion;
char *pigiFilename = NULL;      /* initialized by pigiMain */

void PrintVersion ()
{
    char buf[200];
    Tcl_CmdBuf tclBuf;
    char* command;

    PrintCon(pigiVersion);
    if ( pigiFilename == NULL )
        pigiFilename = "pigiRpc";
    sprintf( buf, "Running %s, %d", pigiFilename, getpid());
    PrintCon(buf);

    tclBuf = Tcl_CreateCmdBuf();
    Tcl_AssembleCmd(tclBuf,"ptkStartupMessage {");
    Tcl_AssembleCmd(tclBuf,pigiVersion);
    Tcl_AssembleCmd(tclBuf,"} {");
    Tcl_AssembleCmd(tclBuf,pigiFilename);
    command = Tcl_AssembleCmd(tclBuf,"}\n");
    Tcl_Eval(ptkInterp, command, 0, (char **) NULL);
}
