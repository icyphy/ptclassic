/* 
    tclMiscTk.c
    Misc. Tk functions that are global in nature

    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
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
