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

/* Version Identification:
 * $Id$
 */
#ifndef _std_h
#define _std_h
/*  _std_h     is defined by the g++ std.h file */

#if defined(hpux) || defined(__hpux) && ! defined(__GNUC__)
#ifndef PTHPPA_CFRONT
#define PTHPPA_CFRONT
#endif
#endif

#if defined(__sgi) && ! defined(__GNUC__)
#ifndef PTIRIX5_CFRONT
/* SGI running IRIX5.x with native SGI CC*/
#define PTIRIX5_CFRONT
#endif
#endif

#if defined(PTHPPA_CFRONT) || defined(SYSV) || defined (SVR4) || defined(PTIRIX5_CFRONT)
#ifdef PTIRIX5_CFRONT
#include <stdio.h>	// dpwe: only way to get sprintf() prototype?
// for open(), used in pigiLoader.cc
#include <sys/stat.h>
#include <fcntl.h>
#endif // PTIRIX5_CFRONT
#include <stdlib.h>
#include <unistd.h>
//#include "../../kernel/miscFuncs.h"
inline long random() { return lrand48();}
inline void srandom(int seed) { srand48(seed);}
#else
// SunOS4.1 CC
#include <stdlib.h>
#include <sysent.h>
#include "../../kernel/miscFuncs.h"
// we could include <strings.h> too, but Sun Cfront 2.1 has an
// incorrect prototype for strcasecmp.
extern "C" int strcasecmp(const char*, const char*);
#endif
#ifdef PTAIX_XLC
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#endif

#endif /* _std_h */
