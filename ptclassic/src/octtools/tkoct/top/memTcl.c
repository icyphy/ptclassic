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
#include "dlMacros.h"
#include "topMem.h"
#include "memCore.h"
#include "memStats.h"


static int
_memStatsCmd( ClientData cld, Tcl_Interp *ip, int argc, char **argv) {
    memCoreStatsShow();
    return TCL_OK;
}

static int
_memQuitCmd( ClientData cld, Tcl_Interp *ip, int argc, char **argv) {
    exit(1);
    /*NOTREACHED*/
    return TCL_OK;
}

static int
_memAbortCmd( ClientData cld, Tcl_Interp *ip, int argc, char **argv) {
    abort();
    exit(2);
    /*NOTREACHED*/
    return TCL_OK;
}

int
memRegisterCmds( ip)
    Tcl_Interp		*ip;
{
    memInitialize();
    Tcl_CreateCommand( ip, "memstats", _memStatsCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "memquit", _memQuitCmd, NULL, NULL);
    Tcl_CreateCommand( ip, "memabort", _memAbortCmd, NULL, NULL);
    return TCL_OK;
}

