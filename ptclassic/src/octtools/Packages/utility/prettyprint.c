#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "port.h"
#include "utility.h"

struct scale {
    double  mul;
    char* label;
};

struct scale scaleTable[] = {
    { 0, 0 },
    { 1e-15, "f"},
    { 1e-12, "p"},
    { 1e-9, "n"},
    { 1e-6, "u"},
    { 1e-3, "m"},
    { 1e0, ""},
    { 1e3, "k"},
    { 1e6, "MEG"},
    { 1e9, "G"},
    { 1e12, "T"},
    { 0, 0}
};

char* util_pretty_print( v )
    double v;
{
    static char buf[8][256];
    static int count = 7;
    double s;
    

    int i = 6;
    
    if ( !count-- ) count = 7;
    if ( v != 0.0 ) {
	while (1) {
	    s = v / scaleTable[i].mul;
	    if ( ABS(s) > 1000.0 && scaleTable[i+1].mul != 0.0) {
		i++;
	    } else if ( ABS(s) < 1e-1   && scaleTable[i-1].mul != 0.0) {
		i--;
	    } else {
		break;
	    }
	} 
    }

    sprintf( buf[count], "%.3g%s", v / scaleTable[i].mul, scaleTable[i].label );
    return buf[count];
}
