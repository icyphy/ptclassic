/* Ptolemy Version: $Id$ */
/* 
 * tkJava.c --
 *
 *	This file implements the native methods for TkMainWindow.
 *
 * Copyright (c) 1995 by Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

static char sccsid[] = "@(#) tkJava.c 1.2 95/12/19 14:34:49";

#include <tk.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

/* Under NT, we need to undefine Status, or we will see lots of error
 * messages while compiling  rpcdce.h
 */
#undef Status
#include <oobj.h>
#include <interpreter.h>
#include <common_exceptions.h>
#include <java_lang_String.h>
#include <javaString.h>
#include <typecodes.h>

#include "tcl_classes.h"

/*
 * Initialize TK.
 */
void
tcl_TkApplication_tkInit(Htcl_TkApplication *tkH,
	Htcl_Interp *interpH, HString *name, HString *class)
{
    Tk_Window mainWindow;
    char buffer[512];

    Tcl_Interp *interp = (Tcl_Interp *) unhand(interpH)->interp;

    sprintf(buffer,"-name %s -class %s",makeCString(name), makeCString(class));
    Tcl_SetVar2(interp,"argv", (char *) NULL, buffer, TCL_GLOBAL_ONLY); 

#ifdef CREATEMAINWINDOWHACK
    mainWindow = Tk_CreateMainWindow(interp, NULL, makeCString(name), "TkJava");
#endif
    
    if (Tk_Init(interp) == TCL_ERROR) {
	SignalError(0, JAVAPKG "TclError", interp->result);
	return;
    }
}

/*
 * Count main windows left.  Should we exit soon or not?
 */
long
tcl_TkApplication_getNumMainWindows(Htcl_TkApplication *tkH)
{
#ifdef CREATEMAINWINDOWHACK
    extern int tk_NumMainWindows;

    return tk_NumMainWindows;
#else
    return Tk_GetNumMainWindows();
#endif
}
