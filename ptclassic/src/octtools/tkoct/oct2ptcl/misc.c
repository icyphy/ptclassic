/*******************************************************************
SCCS version identification
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

 Programmer: Kennard White

    misc.c :: oct2ptcl translator

    Misc. functions to aid the main oct2ptcl functions.  These are 
    "helper" functions that should really be provided by other packages,
    but for some reason they are not.

********************************************************************/

#if !defined(lint) && !defined(SABER)
static char SccsId[] = "$Id$";
#endif

#include "topFixup.h"

#include <stdio.h>
#include <stdarg.h>

#include <tcl.h>

#include "oct.h"
#include "oh.h"
#include "errtrap.h"

#include "topStd.h"
#include "topMem.h"

#include "oct2ptcl.h"

#define SPKG _OtpPackage

char*
Tcl_DStringAppends( Tcl_DString *pStr, ...) {
    va_list	args;
    char	*elStr;

    va_start( args, pStr);
    while (1) {
        elStr = va_arg( args, char *);
	if ( elStr == NULL )
	    break;
	Tcl_DStringAppend( pStr, elStr,-1);
    }
    va_end(args);
    return pStr->string;
}

char*
Tcl_DStringAppendEls( Tcl_DString *pStr, ...) {
    va_list	args;
    char	*elStr;

    va_start( args, pStr);
    while (1) {
        elStr = va_arg( args, char *);
	if ( elStr == NULL )
	    break;
	Tcl_DStringAppendElement( pStr, elStr);
    }
    va_end(args);
    return pStr->string;
}


/**
    Should use uid instead of dynamic memory...
**/
char*
otpCvtPropToStr( octObject *pProp) {
    switch ( pProp->contents.prop.type ) {
    case OCT_INTEGER:
	return memStrSaveFmt("%d",pProp->contents.prop.value.integer);
    case OCT_REAL:
	return memStrSaveFmt("%g",pProp->contents.prop.value.real);
    case OCT_STRING:
	return pProp->contents.prop.value.string;
    default: ;
    }
    errRaise(SPKG,-1,"Cannot convert prop type %d to string.",
      pProp->contents.prop.type);
    return NULL;
}
