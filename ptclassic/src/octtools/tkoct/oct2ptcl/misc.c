/**
    misc.c :: oct2ptcl translator

    Misc. functions to aid the main oct2ptcl functions.  These are 
    "helper" functions that should really be provided by other packages,
    but for some reason they are not.

    Author: Kennard White
**/
#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Header$";
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
