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

#include "PTcl.h"
#include "ganttChart.h"

/* functions added to handle PTcl and pigi compatability */
/* needed to avoid interp calls in cg domain */

void
GanttChart::beginGantt(ostream& out, char *argstring) {

	Tcl_Eval(PTcl::activeInterp, argstring);

}

void
GanttChart::writeGanttLabel(ostream& out, char *argstring) {

	Tcl_Eval(PTcl::activeInterp, argstring);
}

void
GanttChart::writeGanttBindings(ostream& out, char *argstring) {

	Tcl_Eval(PTcl::activeInterp, argstring);
}

void
GanttChart::endGantt(ostream& out) {
}

void
GanttChart::writeGanttProc(ostream& out, char *argstring) {
  
  	Tcl_Eval(PTcl::activeInterp, argstring);
}
