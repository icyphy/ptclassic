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

