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
#include "copyright.h"
#include "port.h"
#include "errtrap.h"
#include "oct.h"
#include "utility.h"
#include "oh.h"
#include "vov.h"


static void getProp( c, n, p, type )
    octObject *c;
    char *n;
    octObject *p;
    octPropType type;
{
    if ( ohGetByPropName( c, p, n ) != OCT_OK ) {
	errRaise( "OH", 1, "No prop %s in %s", n, ohFormatName( c ) );
    }
    if ( p->contents.prop.type != OCT_REAL ) {
	errRaise( "OH", 1, "Wrong type of %s",  ohFormatName( p ) );
    }
}



double ohGetRealProp( c, n )
    octObject *c;
    char *n;
{
    octObject p;
    
    getProp( c, n, &p, OCT_REAL );
    return p.contents.prop.value.real;
}

int ohGetIntegerProp( c, n )
    octObject *c;
    char *n;
{
    octObject p;
    getProp( c, n, &p, OCT_INTEGER );
    return p.contents.prop.value.integer;
}

char* ohGetStringProp( c, n )
    octObject *c;
    char *n;
{
    octObject p;
    getProp( c, n, &p, OCT_STRING );
    return p.contents.prop.value.string;
}

