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
						COPYRIGHTENDKEY

 Programmer:  Christopher X. Hylands
 Date of creation: October 20, 1995

 Definition of constant PTARCH Stripped from pigiLoader.cc.

**************************************************************************/


// Check value of PTARCH
// -- it could be defined by makefiles or other means
// -- also prevents contents of file from being evaluated multiple times

#ifndef PTARCH

// Architecture-specific stuff.  The PTFOO defs come from compat.h
#include "compat.h"

#ifdef PTAIX
#define PTARCH "aix"
#endif

#ifdef PTAIX_XLC
// IBM AIX running IBM's c and c++ compilers
#define PTARCH "aix.xlc"
#endif

#ifdef PTALPHA
#define PTARCH "alpha"
#endif

#ifdef PTFREEBSD
#define PTARCH "freebsd"
#endif

#ifdef PTHPPA
#define PTARCH "hppa"
#endif

#ifdef PTHPPA_CFRONT
#undef PTARCH
#define PTARCH "hppa.cfront"
#endif

#ifdef PTIRIX5
#define PTARCH "irix5"
#endif

#ifdef PTIRIX5_CFRONT
#undef PTARCH
#define PTARCH "irix5.cfront"
#endif

#ifdef PTLINUX
#define PTARCH "linux"
#endif

#ifdef PTULTRIX
#define PTARCH "mips"
#endif

#ifdef __mc68000__
#define PTARCH "sun3"
#endif

#ifdef PTSOL2
#define PTARCH "sol2"
#endif

#ifdef PTSOL2_CFRONT
#undef PTARCH
#define PTARCH "sol2.cfront"
#endif

#ifdef PTSUN4
#define PTARCH "sun4"
#endif

#ifdef PTSVR4
#undef PTARCH
#define PTARCH "unixware"
#endif

#ifdef vax
#define PTARCH "vax"
#endif

#endif

// End of check value of PTARCH
