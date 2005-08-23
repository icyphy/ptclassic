#ifndef _PTK_H
#define _PTK_H 1

/* 
Version: @(#)ptk.h	1.12 01/28/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

/* Used <tcl.h> and <tk.h>, not "tcl.h" and "tk.h" so that
 * make depend does not depend on the tcl.h and tk.h files, which could
 * be located anywhere.
 */
#include <tcl.h>

/* Note that if we are including ptk.h in a C++ file, we must include
 * ptk.h last because ptk.h includes tk.h which eventually includes
 * X11/X.h, which on Solaris2.4 there is a #define Complex 0, which
 * causes no end of trouble.
 */
#include <tk.h>

/* Unfortunately X.h which is included by tk.h defines Complex to "0".
   The following fix permits star files to use class Complex.
*/
#if Complex == 0
#   undef Complex
#endif

#ifdef __cplusplus
extern "C" Tcl_Interp *ptkInterp;
#else
extern Tcl_Interp *ptkInterp;
#endif

extern Tk_Window ptkW;

#endif   /* _PTK_H */
