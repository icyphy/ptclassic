/* 
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
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
