/**************************************************************************
Version identification:
@(#)miscFuncs.h	1.23	01/05/99

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

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

 This header file provides prototypes for miscellaneous
 library functions that are not members, together with some
 commonly used library functions.

*************************************************************************/

#ifndef _miscFuncs_h
#define _miscFuncs_h 1
#ifdef __GNUG__
#pragma interface
#endif


#if (defined(sun) && (defined(__svr4__) || defined(SVR4) || defined(SYSV))) || defined(PTSVR4)
/* Solaris2.3 defines type boolean as an enum, which is a no no
 * See also octtools/include/port/port.h
 * We can't just include "port.h", or we will need to add
 * -I$(OCTTOOLS)/include in a lot of places.  The tcltk stars include
 * pigilib header files.
 */
#define boolean sun_boolean
#include <sys/types.h>
#undef boolean
#include <std.h>
#else
/* std.h eventually includes types.h, which causes problems with SDF
 * stars under Solaris2.3
 */
#include <std.h>
#endif

#ifdef PT_NT4VC
#include <io.h>
#include <sys/stat.h> /* For creat in Display.cc */
#include <stdlib.h> /* For system in Display.cc */
#undef min	      /* Undef min and max for nt4.vc */
#undef max
#endif

#include "logNew.h"
#include "Error.h"
#include <string.h>

extern "C" double pow(double,double);

// return an expanded version of the filename argument, which may
// start with "~", "~user", or "$var".  The returned value points
// to dynamic storage which should be freed using delete [] when no
// longer used.
char* expandPathName(const char*);

// return a new, unique temporary file name.  The returned value points
// to dynamic storage which should be freed using delete [] when no
// longer used.
char* tempFileName();

// return a new string formed by replacing each character "target"
// in string "str" with string "substr".  The returned value points
// to dynamic storage which should be freed using delete [] when no
// longer used.
char* subCharByString(const char *str, char target, const char *substr);

// return a new copy of the string 'txt'.  The returned value points
// to dynamic storage which should be freed using delete [] when no
// longer used.
inline char* savestring (const char* txt) {
  if (txt) {
    INC_LOG_NEW; return strcpy (new char[strlen(txt)+1], txt);
  }
  else {
    Error::warn("savestring() passed NULL instead of a string");
    return strcpy (new char[strlen("")+1], "");
  }
}

// Save the string in a systemwide hash table, if it is not there
// already, and return a pointer to the saved copy.  Subsequent
// hashstrings with the same string return the same address.
// Do not delete strings returned by hashstring.
const char* hashstring(const char*);

// I would prefer to make this int pow(int,int), but g++ gets confused
// because the other function is extern "C".
int power(int base,int exp);
inline double power(double base,double exp) { return pow(base,exp);}

#endif
