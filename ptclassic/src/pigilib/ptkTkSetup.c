/* 
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
                                                        COPYRIGHTENDKEY
*/

/*
    ptkTkSetup.c  aok
    Version: @(#)ptkTkSetup.c	1.3 6/30/93
*/


/* 
  This file creates the Tcl Interpreter, Tk Main Window, and 
  creates the RPC connection from Tcl to RPC - Alan Kamas
*/
   
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

int
ptkTkSetup(funcArray, size)
    RPCFunction *funcArray;
    long size;
{
    static RPCClientData RPCdata;
    static char buf[256];

    int result;

    RPCdata.funcArray = funcArray;
    RPCdata.size = size;

    /* Create Tk Window */
    ptkInterp = Tcl_CreateInterp();
    ptkW = Tk_CreateMainWindow(ptkInterp, NULL, "pigi");
    if (ptkW == NULL) {
	ErrAdd("FATAL ERROR");
	ErrAdd(ptkInterp->result);
	PrintErr(ErrGet());
        exit(1);
    }
    Tk_SetClass(ptkW, "Pigi");

    /* Register all Tk Functions here */
    ptkRegisterCmds( ptkInterp, ptkW);

    Tk_CreateFileHandler(fileno(RPCReceiveStream), TK_READABLE,
       ptkRPCFileProc, (ClientData) &RPCdata);

    sprintf(buf, "source %s/tcl/lib/ptkInit.tcl",getenv("PTOLEMY"));
    if (Tcl_Eval(ptkInterp, buf, 0, (char **)NULL) != TCL_OK) {
        ErrAdd("Unable to load $PTOLEMY/tcl/lib/ptkInit.tcl file: ");
        ErrAdd(ptkInterp->result);
        PrintErr(ErrGet());
	exit(1);
    }

    /* pop up the welcome window */
    PrintVersion();

    /* arrange for "processEvents" to be called between stars */
    KcSetEventLoop(1);

    return (RPC_OK);
}

int
ptkMainLoop ()
{
    Tk_MainLoop ();
}


void
processEvents (s,c)
struct Star* s;
char *c;
{
	/* FIXME: use TK_DONT_WAIT */
	Tk_DoOneEvent(1);
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


