/* 
Copyright (c) 1990-1998 The Regents of the University of California.
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

/*
    ptkTkSetup.c  aok
    Version: @(#)ptkTkSetup.c	1.30	09/23/98
*/


/* 
  This file creates the Tcl Interpreter, Tk Main Window, and 
  creates the RPC connection from Tcl to RPC
*/
   
#include "pthreadsCompat.h"	/* Must be first for hpux10 */
/* Standard includes */
#include <stdio.h>

#ifndef PT_NO_ITCL
#include "itcl.h"
#endif /* PT_NO_ITCL */

#ifdef ITCL_VERSION
/* ITCL2.0 or better */
#include "itk.h"
#endif

#include "local.h"			/* include "ansi.h" and "compat.h" */

/* Octtools includes */
#include "rpc.h"
#include "rpcApp.h"

/* Pigilib includes */
#include "ptk.h"
#include "ptkRegisterCmds.h"		/* define ptkRegisterCmds */
#include "ptkNet.h"			/* define ptkRPCFileHandler */
#include "err.h"			/* define ErrAdd */


/* Used to pass the funcArray through Tk */
typedef struct {
RPCFunction *funcArray;
long size;                      /* number of items in the array */
} RPCClientData;

/* 
   Stuffs Arguments in to structure, coverts structure to clientdata
   Sets up TkLoop Call
*/

#include "xfunctions.h"         /* define prototype for PrintVersion */
#include "vemInterface.h"       /* define prototype for PrintErr */
#include "kernelCalls.h"	/* define prototype for KcSetEventLoop */


static void
ptkRPCFileProc ( clientdata, mask )
ClientData clientdata;
int mask;
{
    RPCClientData *RPCData = (RPCClientData *) clientdata;
    RPCFunction *funcArray = RPCData->funcArray; 
    long size = RPCData->size;

    if (ptkRPCFileHandler( funcArray, size ) != RPC_OK ) {
	fprintf(stderr, "RPC connection to VEM closed.\n");
	exit(1);
    }

}


static int
_ptkAppInit( ip)
    Tcl_Interp *ip;
{
    if (Tcl_Init(ip) == TCL_ERROR)
	return TCL_ERROR;
    if (Tk_Init(ip) == TCL_ERROR)
	return TCL_ERROR;
    /* Add [incr Tcl] (itcl) facilities */
#ifndef PT_NO_ITCL
    if (Itcl_Init(ip) == TCL_ERROR) {
      return TCL_ERROR;
    }

#ifdef ITCL_VERSION
    /* ITCL2.0 or better */
    if (Itk_Init(ip) == TCL_ERROR) {
      return TCL_ERROR;
    }
#endif

#if ITCL_MAJOR_VERSION >= 3
    Tcl_StaticPackage(ip, "Itcl", Itcl_Init, Itcl_SafeInit);
    Tcl_StaticPackage(ip, "Itk", Itk_Init, (Tcl_PackageInitProc *) NULL);

    /*
     *  This is itclsh, so import all [incr Tcl] commands by
     *  default into the global namespace.  Set the "itcl::native"
     *  variable so we can do the same kind of import automatically
     *  during the "auto_mkindex" operation.
     */
    if (Tcl_Import(ip, Tcl_GetGlobalNamespace(ip),
            "::itk::*", /* allowOverwrite */ 1) != TCL_OK) {
        return TCL_ERROR;
    }

    if (Tcl_Import(ip, Tcl_GetGlobalNamespace(ip),
            "::itcl::*", /* allowOverwrite */ 1) != TCL_OK) {
        return TCL_ERROR;
    }

    if (!Tcl_SetVar(ip, "::itcl::native", "1", TCL_LEAVE_ERR_MSG)) {
        return TCL_ERROR;
    }
#endif /* ITCL_MAJOR_VERSION */
#endif /* PT_NO_ITCL */

    ptkRegisterCmds(ip);
    return TCL_OK;
}

int
ptkTkSetup(funcArray, size)
    RPCFunction *funcArray;
    long size;
{
    static RPCClientData RPCdata;
    static char buf[256];
    char *appName = "pigi";
    char *appClass = "Pigi";
    char *pt;

    RPCdata.funcArray = funcArray;
    RPCdata.size = size;

    /* Create Tk Window */
    ptkInterp = Tcl_CreateInterp();
#if TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1
    ptkW = Tk_CreateMainWindow(ptkInterp, NULL, appName, appClass);
    if (ptkW == NULL) {
	ErrAdd("FATAL ERROR");
	ErrAdd(ptkInterp->result);
	ErrAdd(Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
	PrintErr(ErrGet());
        exit(1);
    }
#endif /* TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1 */

    /* no argc&argv to transfer */
    Tcl_SetVar(ptkInterp, "argv0", "pigi", TCL_GLOBAL_ONLY);

    Tcl_SetVar(ptkInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);

    /* our vers of Tk_AppInit */
    if (_ptkAppInit( ptkInterp) != TCL_OK) {
        ErrAdd("_ptkAppInit failed: ");
        ErrAdd(ptkInterp->result);
	ErrAdd(Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
	PrintErr(ErrGet());
	exit(1);
    }
			 
#if (TK_MAJOR_VERSION >= 4 && TK_MINOR_VERSION >= 1) || TK_MAJOR_VERSION >= 8
    ptkW = Tk_MainWindow(ptkInterp);
    if (ptkW == NULL) {
	ErrAdd("FATAL ERROR");
	ErrAdd(ptkInterp->result);
	ErrAdd(Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
	PrintErr(ErrGet());
        exit(1);
    }
    Tk_SetClass(ptkW, appClass);
    Tk_SetAppName(ptkW, appName);

#if TK_MAJOR_VERSION >= 8
    {
        /* See the following post:
         * Subject:      Re: transition from obsolete Tcl_File
         * From:         Marc Graham <marc@neovision.com>
         * Newsgroups:   comp.lang.tcl 
         */

        Tcl_Channel pipe_channel;

        pipe_channel = Tcl_MakeFileChannel(
                (ClientData) fileno (RPCReceiveStream),   
                TCL_READABLE | TCL_EXCEPTION);
        Tcl_RegisterChannel(ptkInterp, pipe_channel);
        /* FIXME: in theory, we should be unregistering this somewhere */
        Tcl_CreateChannelHandler (pipe_channel, TCL_READABLE, ptkRPCFileProc,
                            (ClientData) &RPCdata);
    }
#else
    Tcl_CreateFileHandler(Tcl_GetFile((ClientData)
				      ((int)fileno(RPCReceiveStream)),
				      TCL_UNIX_FD),
			  TCL_READABLE, ptkRPCFileProc, (ClientData) &RPCdata);
#endif /* TK_MAJOR_VERSION >= 8 */
#else
    Tk_CreateFileHandler(fileno(RPCReceiveStream), TK_READABLE,
    			 ptkRPCFileProc, (ClientData) &RPCdata);
#endif /* (TK_MAJOR_VERSION >= 4 && TK_MINOR_VERSION >= 1) || TK_MAJOR_VERSION >= 8 */

    pt = getenv("PTOLEMY");
    sprintf(buf, "%s/lib/tcl/pigilib.tcl", pt ? pt : "~ptolemy");
    if (Tcl_EvalFile(ptkInterp, buf) != TCL_OK) {
        ErrAdd("Unable to load ptk startup file: ");
        ErrAdd(ptkInterp->result);
	ErrAdd(Tcl_GetVar(ptkInterp,"errorInfo",TCL_GLOBAL_ONLY));
	PrintErr(ErrGet());
	exit(1);
    }

    /* pop up the welcome window */
    PrintVersion();

    /* arrange events to be called during a Ptolemy run*/
    KcSetEventLoop(1);

    return (RPC_OK);
}

void
ptkMainLoop ()
{
    Tk_MainLoop ();
}


