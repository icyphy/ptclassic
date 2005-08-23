/*
Shared header file for Ptolemy code that interfaces to Tcl, Tk, or Itcl.

@(#)ptk.h	1.2	03/26/98

Copyright (c) 1990-1995 The Regents of the University of California.
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
***********************************************************************/

/* the following fixes permit the "DECwindows" version of Xlib.h to work with
   C++ */

struct XSizeHints;
struct XStandardColormap;
struct XTextProperty;
struct XWMHints;
struct XClassHint;

#include "tcl.h"
#include "tk.h"
#include "itcl.h"

/* Unfortunately X.h which is included by tk.h defines Complex to "0".
   The following fix permits star files to use class Complex.
*/

#if Complex == 0
#   undef Complex
#endif

extern Tcl_Interp *ptkInterp;

/*
 * Note that in pigi, the following was not a pointer.
 * We make it a pointer here to allow multiple interpreters with multiple
 * console windows to coexist.
 */
extern Tk_Window &ptkW;

