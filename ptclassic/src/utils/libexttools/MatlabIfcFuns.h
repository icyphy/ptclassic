#ifndef _MatlabIfcFuns_h
#define _MatlabIfcFuns_h 1

/**************************************************************************
Version identification:
@(#)MatlabIfcFuns.h	1.5	07/09/97

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

 Programmer:  Brian L. Evans
 Date of creation: 11/6/95
 Revisions:

 C include file for the functions supporting the Ptolemy interface to
 Matlab.

**************************************************************************/

/* define ARGS macro */
#include "compat.h"

/* matrix.h and engine.h are not actually system include files, but if
   we refer to them as "matrix.h" and "engine.h", then make depend
   will include them in the makefile.  If a user does not have Matlab,
   then they will have to regenerate a makefile because the makefile
   refers to Matlab files that they do not have.  A negative side
   effect of this is that if these files change, then this file will
   not be automatically recompiled (cxh) */

/* For Matlab 3.0, matrix.h relies on the data type 'bool' being defined.
   This data type is not properly handled by some cfront (non-GNU)
   compilers.  For example, the Solaris 2.5 CC compiler says that
   bool is undefined, and the HP-PA CC compiler warns that "bool" is a
   future reserved word*. (ble,cxh) */
 
#ifndef __GNUC__
#define bool int
#endif

/* Use the constant REAL to check for Matlab 4 */
#ifdef REAL
#undef REAL
#endif

#include <matrix.h>
#include <engine.h>

#define MXCONST const

/* Backward compatibility with interface to Matlab 4.2 */

#ifdef REAL

/* Constants */
#undef  COMPLEX
#undef  REAL
#undef  TEXT
#undef  MATRIX
#define mxREAL     0
#define mxCOMPLEX  1

/* Datatypes */
#undef  MXCONST
#define MXCONST
#ifndef bool
#define bool int
#endif
typedef Matrix mxArray;
typedef int mxComplexity;

/* Functions */
#define mxDestroyArray mxFreeMatrix
#define engGetArray engGetMatrix
#define engPutArray engPutMatrix
#define mxCreateDoubleMatrix mxCreateFull
#endif

#define MXMATRIX   0
#define MXTEXT     1

#endif
