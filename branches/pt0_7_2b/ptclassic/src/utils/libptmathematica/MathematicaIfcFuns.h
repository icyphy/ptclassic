#ifndef _MathematicaIfcFuns_h
#define _MathematicaIfcFuns_h 1

/**************************************************************************
Version identification:
@(#)MathematicaIfcFuns.h	1.5	07/09/97

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
 Date of creation: 1/18/96
 Revisions:

 C include file for the functions supporting the Ptolemy interface to
 Mathematica.

**************************************************************************/

/* define ARGS macro */
#include "compat.h"

/* mathlink.h is not actually a system include file, but if we refer to
   it as "mathlink.h", then make depend will include them in the makefile.
   If a user does not have Mathematica, then they will have to regenerate
   a makefile because the makefile refers to Mathematica files that they
   do not have.  A negative side effect of this is that if these files
   change, then this file will not be automatically recompiled (cxh) */

/* Create typedefs to define Mathematica 2.2 as Mathematica 3.0 types */
#define USING_OLD_TYPE_NAMES 1

/* mathlink.h is a C include file */
#ifdef __cplusplus
extern "C" {
#endif
#include <mathlink.h>
#ifdef __cplusplus
}
#endif

#define PTMLLONG long

/* Backwards compatibility with Mathematica 2.2 */

#ifndef MLVERSION
#undef PTMLLONG 
#define PTMLLONG int
typedef int mldlg_result;

typedef ml_charp  kcharp_ct;
typedef ml_charpp kcharpp_ct;
typedef unsigned long ulong_ct;
#endif

#endif
