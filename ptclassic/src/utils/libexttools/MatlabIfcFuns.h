#ifndef _MatlabIfcFuns_h
#define _MatlabIfcFuns_h 1

/**************************************************************************
Version identification:
$Id$

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

 Programmer:  Brian L. Evans
 Date of creation: 11/6/95
 Revisions:

 C++ include file for the functions supporting the Ptolemy interface to
 Matlab.

**************************************************************************/

// matrix.h and engine.h are not actually system include files, but if
// we refer to them as "matrix.h" and "engine.h", then make depend
// will include them in the makefile.  If a user does not have Matlab,
// then they will have to regenerate a makefile because the makefile
// refers to Matlab files that they do not have.  A negative side
// effect of this is that if these files change, then this file will
// not be automatically recompiled (cxh)
#include <matrix.h>
#include <engine.h>

// Give Matlab's definition of COMPLEX and REAL different names
#undef  COMPLEX
#undef  REAL
#undef  TEXT
#undef  MATRIX

#define MXCOMPLEX  1
#define MXREAL     0
#define MXTEXT     1
#define MXMATRIX   0

#endif
