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
#include <tcl.h>

#include "topStd.h"
#include "topTcl.h"

void
xpRegisterDataHandler( Tcl_Interp *ip, char *name, char *class, TOPPtr pDH) {
    char        buf[100];

    sprintf( buf, "0x%x", (unsigned)pDH);
    Tcl_SetVar2( ip, class, name, buf, TCL_GLOBAL_ONLY);
}


/**
    Returns NULL on error, otherwise a pointer to the data handler structure.
**/
TOPPtr
xpGetDataHandler( Tcl_Interp *ip, char *name, char *class, TOPPtr defVal) {
    char	*dhAddrStr;
    TOPPtr	pDH = NULL;

    if ( name == NULL || name[0] == '\0' ) {
	pDH = defVal;
    } else {
	dhAddrStr = Tcl_GetVar2( ip, class, name, TCL_GLOBAL_ONLY);
	if ( dhAddrStr == NULL ) {
	    Tcl_AppendResult(ip,"No data handler registered for data src ``",
	      name,"'', class ``",class,"''",NULL);
	    return NULL;
	}
	if ( Tcl_GetInt( ip, dhAddrStr, (int*)&pDH) != TCL_OK ) {
	    Tcl_AppendResult(ip,"\nBad address for data src ``",
	      name,"''",NULL);
	    return NULL;
	}
    }
    return pDH;
}
