/* 
    tclMiscTk.c
    Misc. Tk functions that are global in nature

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
#include <tk.h>

#include "topStd.h"
#include "topMem.h"
#include "topTcl.h"

int
topTkMapCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    Tk_Window		tkw;

    if ( aC != 2 ) {
	Tcl_AppendResult(ip,"usage: ", aV[0], " window", NULL);
	return TCL_ERROR;
    }
    if ( (tkw = Tk_NameToWindow(ip, aV[1], appwin)) == NULL ) {
	return TCL_ERROR;
    }
    Tk_MapWindow(tkw);
    return TCL_OK;
}

int
topTkUnmapCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    Tk_Window		tkw;

    if ( aC != 2 ) {
	Tcl_AppendResult(ip,"usage: ", aV[0], " window", NULL);
	return TCL_ERROR;
    }
    if ( (tkw = Tk_NameToWindow(ip, aV[1], appwin)) == NULL ) {
	return TCL_ERROR;
    }
    Tk_UnmapWindow(tkw);
    return TCL_OK;
}

int
topTkDepthCmd( ClientData cld, Tcl_Interp *ip, int aC, char **aV) {
    Tk_Window		appwin = (Tk_Window) cld;
    char		buf[100];

    sprintf( buf, "%d", DefaultDepthOfScreen(Tk_Screen(appwin)));
    Tcl_SetResult(ip,buf,TCL_VOLATILE);
    return TCL_OK;
}

int
topTkMiscRegisterCmds( Tcl_Interp *ip, Tk_Window appwin) {
   Tcl_CreateCommand( ip, "windowmap", topTkMapCmd,
     (ClientData)appwin, NULL);
   Tcl_CreateCommand( ip, "windowunmap", topTkUnmapCmd,
     (ClientData)appwin, NULL);
   Tcl_CreateCommand( ip, "windowdepth", topTkDepthCmd,
     (ClientData)appwin, NULL);
   return TCL_OK;
}
