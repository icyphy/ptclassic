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
