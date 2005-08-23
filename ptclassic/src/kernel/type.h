/**************************************************************************
Version identification:
@(#)type.h	1.15	12/31/95

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

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

 Basic system definitions.

**************************************************************************/
#ifndef _type_h
#define _type_h 1

#include "logNew.h"

// The most general type of pointer, for use in generic types.
typedef void* Pointer;

// The following is also defined in gnu stdio.h, which is included
// by stream.h
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Older C++ compilers make no distinction between prefix and postfix
// operator++.  For such compilers, POSTFIX_OP should be defined as the
// empty string.  Newer C++ compilers use operator++() as the prefix
// form (++obj) and operator++(int) as the postfix form.  For these we
// have POSTFIX_OP defined to be int.  This is the default, except for
// g++, since newer g++'s accept the prefix form for backward compatibility.

#ifndef POSTFIX_OP
//#ifdef __GNUG__
//#define POSTFIX_OP
//#else
#define POSTFIX_OP int
//#endif
#endif

#endif
