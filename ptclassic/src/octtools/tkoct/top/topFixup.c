/* 
    Author: Kennard White

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
#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"
#include <math.h>
#include "topStd.h"

/*global*/ int TOPFixupDummy = 0;	/* dummy data to make compilier happy */

#if defined(__mips__)
/* round to nearest int value, halfway cases round to larger magnitude */
int nint( double x) {
    return x >= 0 ? floor(x+0.5) : ceil(x-0.5);
}
#endif

#if defined(__mips__)
double exp2( double x) {
    /* 2^x = (e^a)^x = e^(a*x); a = ln 2 */
    return exp( 0.6931471806 * x);
}
#endif

#if defined(__mips__)
double exp10( double x) {
    /* 10^x = (e^a)^x = e^(a*x); a = ln 10 */
    return exp( 2.302585093 * x);
}
#endif

#if defined(__mips__)
/* this is a fortran math function is used by the cpoly routine */
double pow_di( double *x, int *y) {
    return pow( *x, (double) *y);
}
#endif

#if defined(__mips__)
void sincos( double x, double *s, double *c) {
    *s = sin(x);
    *c = cos(x);
}
#endif
