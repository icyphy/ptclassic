/* 
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

/*
    ptkTkSetup.c  aok
    Version: $Id$
*/


/* 
  This file creates the Tcl Interpreter, Tk Main Window, and 
  creates the RPC connection from Tcl to RPC
*/
   
#include <stdio.h>
#include <itcl.h>
#ifdef ITCL_VERSION
/* ITCL2.0 or better */
#include <itk.h>
#endif
#include "compat.h"
#include "ptk.h"
#include "ptkRegisterCmds.h"
#include "ptkNet.h"
#include "rpc.h"
#include "rpcApp.h"
#include "err.h"

static void ptkRPCFileProc ();

/* Used to pass the funcArray through Tk */
typedef struct {
RPCFunction *funcArray;
long size;                      /* number of items in the array */
} RPCClientData;

/* 
   Stuffs Arguments in to structure, coverts structure to clientdata
   Sets up TkLoop Call
*/

void PrintVersion();
extern void KcSetEventLoop(/*int on*/);
extern void PrintErr();

static int
_ptkAppInit( ip, win)
    Tcl_Interp *ip;
    Tk_Window	win;
{
    if (Tcl_Init(ip) == TCL_ERROR)
	return TCL_ERROR;
    if (Tk_Init(ip) == TCL_ERROR)
	return TCL_ERROR;
    /* Add [incr Tcl] (itcl) facilities */
    if (Itcl_Init(ip) == TCL_ERROR) {
      return TCL_ERROR;
    }
#ifdef ITCL_VERSION
    /* ITCL2.0 or better */
    if (Itk_Init(ip) == TCL_ERROR) {
      return TCL_ERROR;
    }
#endif
    ptkRegisterCmds( ip, win);
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
    ptkW = Tk_CreateMainWindow(ptkInterp, NULL, appName, appClass);
    if (ptkW == NULL) {
	ErrAdd("FATAL ERROR");
	ErrAdd(ptkInterp->result);
	PrintErr(ErrGet());
        exit(1);
    }
    /* no argc&argv to transfer */
    Tcl_SetVar(ptkInterp, "argv0", "pigi", TCL_GLOBAL_ONLY);

    Tcl_SetVar(ptkInterp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);

    /* our vers of Tk_AppInit */
    if (_ptkAppInit( ptkInterp, ptkW) != TCL_OK) {
	PrintErr(ptkInterp->result);
	exit(1);
    }

    Tk_CreateFileHandler(fileno(RPCReceiveStream), TK_READABLE,
       ptkRPCFileProc, (ClientData) &RPCdata);

    pt = getenv("PTOLEMY");
    sprintf(buf, "%s/lib/tcl/pigilib.tcl", pt ? pt : "~ptolemy");
    if (Tcl_EvalFile(ptkInterp, buf) != TCL_OK) {
        ErrAdd("Unable to load ptk startup file: ");
        ErrAdd(ptkInterp->result);
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


