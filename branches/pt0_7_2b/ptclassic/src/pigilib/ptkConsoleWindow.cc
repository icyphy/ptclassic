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
    ptkConsoleWindow.cc  aok
    Version: $Id$
*/

// Sets up the standard input to feed from the xterm that
// started up pigi and feeds stdout to that same window.

// Alan Kamas 

#include <stdio.h>
extern "C" {
#include "ptk.h"
}


Tcl_CmdBuf ptkConsoleBuffer;

// The following function is taken directly from Wish.c
void
StdinProc(ClientData clientData, int mask)
{
    char line[200];
    static int gotPartial = 0;
    char *cmd;
    int result;

    if (mask & TK_READABLE) {
        if (fgets(line, 200, stdin) == NULL) {
            if (!gotPartial) {
                Tcl_Eval(ptkInterp, "destroy .", 0, (char **) NULL);
                return;
            } else {
                line[0] = 0;
            }
        }
        cmd = Tcl_AssembleCmd(ptkConsoleBuffer, line);
        if (cmd == NULL) {
            gotPartial = 1;
            return;
        }
        gotPartial = 0;
        result = Tcl_RecordAndEval(ptkInterp, cmd, 0);
        if (*ptkInterp->result != 0) {
            printf("%s\n", ptkInterp->result);
        }
        printf("ptk: ");
        fflush(stdout);
    }
}


void ptkConsoleWindow()
{
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
    printf("ptk started\n\nptk: ");
    fflush (stdout);
    // fixme: should be some way to deletle this when done.
    ptkConsoleBuffer = Tcl_CreateCmdBuf();

}


