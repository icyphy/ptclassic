#ifndef _sol2compat_h
#define _sol2compat_h 1
/**************************************************************************
Version identification:
$Id$
Author: Christopher Hylands

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


#if (defined(sun) && (defined(__svr4__) || defined(SVR4) || defined(SYSV)))
/* Solaris2.3 defines type boolean as an enum, which is a no no
 * See also octtools/include/port/port.h
 * We can't just include "port.h", or we will need to add
 * -I$(OCTTOOLS)/include in a lot of places.  The tcltk stars include
 * pigilib header files.
 */
#define boolean sun_boolean
#include <sys/types.h>
#undef boolean
#else
#include <sys/types.h>
#endif /*sun && __svr4__*/

#ifndef TYPEDEF_BOOLEAN
#define TYPEDEF_BOOLEAN 1
typedef int boolean;
#endif /*TYPEDEF_BOOLEAN*/

#endif /*_sol2compat_h*/
