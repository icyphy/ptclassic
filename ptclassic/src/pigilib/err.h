#ifndef _ERR_H
#define _ERR_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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

/* err.h  edg
Version identification:
@(#)err.h	1.11	4/26/96
Header for error handing routines.
*/

#include "compat.h"

#define ERR_IF1(cond) \
    if (cond) { \
	return(FALSE); \
    }

#define ERR_IF2(cond,msg) \
    if (cond) { \
	ErrAdd(msg); \
	return(FALSE); \
    }

#define ERR_IF3(cond,msg,retVal) \
    if (cond) { \
	ErrAdd(msg); \
	return(retVal); \
    }

/* The following provides more convenient and consistent access to Tcl
   with error checking.  */
#define TCL_CATCH_ERR(tcl_call) \
    if ( (tcl_call) != TCL_OK) { \
	Tcl_Eval(ptkInterp,"ptkDisplayErrorInfo"); \
    }

#define TCL_CATCH_ERR1(tcl_call) \
    if ( (tcl_call) != TCL_OK) { \
	Tcl_Eval(ptkInterp,"ptkDisplayErrorInfo"); \
	return FALSE; \
    }

#define TCL_CATCH_ERR1_BREAK(tcl_call) \
    if ( (tcl_call) != TCL_OK) { \
	Tcl_Eval(ptkInterp,"ptkDisplayErrorInfo"); \
	break; \
    }

#define TCL_CATCH_ERR2(tcl_call,retVal) \
    if ( (tcl_call) != TCL_OK) { \
	Tcl_Eval(ptkInterp,"ptkDisplayErrorInfo"); \
	return retVal; \
    }

extern void ErrClear();
extern void ErrAdd ARGS((const char* msg));
extern char* ErrGet();

#endif   /* _ERR_H */
