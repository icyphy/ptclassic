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
*/
/* Routines for talking to the Gantt chart display */
/* $Id$ */

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

#include "local.h"
#include "err.h"
#include "vemInterface.h"
#include "xfunctions.h"
#include "PTcl.h"

extern PTcl *ptcl;

/* See libgantt/gantt_main.c */
extern "C" int display_schedule( /* char *display_name, int PIGI_present,
				char *input_file, char *output_file */);


extern "C" {
boolean
displayGanttChart(char* fileName)
{
		return (TRUE);
}
}

/* 8/12/89
Save the error message for GGI and exit from display_schedule().
*/
extern "C" {
void
GanttErr(char* msg)
{
}
}

extern "C" {
int
GanttMan(char* name)			/* ARGSUSED */
{
    return 0;
}
}

/* functions added to handle ptcl and pigi compatability */
/* needed to avoid interp calls in cg domain */

extern void beginGantt(ostream& out, char *argstring) {

	Tcl_Eval(ptcl->activeInterp, argstring);

}

extern void writeGanttLabel(ostream& out, char *argstring) {

	Tcl_Eval(ptcl->activeInterp, argstring);
}

extern void writeGanttBindings(ostream& out, char *argstring) {

	Tcl_Eval(ptcl->activeInterp, argstring);
}

extern void endGantt(ostream& out) {
}

extern void writeGanttProc(ostream& out, char *argstring) {

	Tcl_Eval(ptcl->activeInterp, argstring);
}


