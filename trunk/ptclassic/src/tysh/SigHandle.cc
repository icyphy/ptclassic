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

/*****************************************************************************
Version identification:
$Id$

Authors: Joel R. King

Defines the signal handlers for Tycho.
Some refinement by Brian Evans.

*****************************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>		
#include "compat.h"
#include "ptarch.h"
#include "tcl.h"
#include "SigHandle.h"

// Maximum path name length in characters
#define MAX_PATH_LENGTH  512

// Sub-directory of $PTOLEMY that contains the itcl binary plus binary
// file name, e.g., "bin.sol2/itkwish"
// ANSI C will concatenate adjacent strings
#define SH_ITCLTK_PROG ("/bin." PTARCH "/itkwish")

// Sub-directory of $PTOLEMY that contains itcl file to source plus file name
#define SH_ITCLTK_FILE "/tycho/kernel/TyCoreRelease.itcl"

// Used so we can call the Tcl procedure emergencySave
extern Tcl_Interp *ptkInterp;

// An array of pointers to strings containing the environment variables
// Needed by the execle() function to setup the process it is launching
extern "C" char **environ;


// DoTychoSave
//
// This function calls the Tcl method emergencySave, which attempts to do 
// an emergency save of all active files with names. It is a method of the
// class TyConsole and we invoke it for the object .mainConsole because we
// know this object is alive as long as the program is.

static void DoTychoSave(void)
{
    Tcl_VarEval(ptkInterp, ".mainConsole emergencySave", NULL);
}    


// setHandlers
//
// This function sets the signal handler function for each of the
// signals that we want to intercept.

int setHandlers(SIG_PF sigHandler)
{
    if (ptSignal(SIGBUS, sigHandler) != 0) {
        return 1;
    }
    if (ptSignal(SIGEMT, sigHandler) != 0) {
        return 2;
    }
    if (ptSignal(SIGFPE, sigHandler) != 0) {
        return 3;
    }
    if (ptSignal(SIGILL, sigHandler) != 0) {
        return 4;
    }
    if (ptSignal(SIGIOT, sigHandler) != 0) {
        return 5;
    }
    if (ptSignal(SIGQUIT, sigHandler) != 0) {
        return 6;
    }
    if (ptSignal(SIGSEGV, sigHandler) != 0) {
        return 7;
    }
    if (ptSignal(SIGSYS, sigHandler) != 0) {
        return 8;
    }
    if (ptSignal(SIGTRAP, sigHandler) != 0) {
        return 9;
    }
#ifndef PTHPPA
    if (ptSignal(SIGXCPU, sigHandler) != 0) {
        return 10;
    }
    if (ptSignal(SIGXFSZ, sigHandler) != 0) {
        return 11;
    }
#endif // PTHPPA
    return 0;
}


// signalHandlerRelease
//
// This function defines the way that a signal that is received will be
// handled when PT_DEVELOP is not defined, or 0.  The action of this
// function is to issue an error message letting the user know that a
// fatal error occured, then exit.

void signalHandlerRelease(void)
{
    // We check this in case the call to DoTychoSave bombs 
    // out and generates another signal. 
    static int times = 0;
    if (times == 0) {               
        times++;
	DoTychoSave();
    }

    // Use execle to execute the Unix command "$path $file -name Tycho" where
    //   path = $PTOLEMY/bin.$PTARCH/itkwish
    //   file = $PTOLEMY/tycho/kernel/TyCoreRelease.itcl
    // Cannot use dynamic memory
    static char path[MAX_PATH_LENGTH], file[MAX_PATH_LENGTH];

    // 1. Find the value of the PTOLEMY environment variable
    const char *ptolemy = getenv("PTOLEMY");
    if ( ptolemy == 0 ) ptolemy = "~ptolemy";

    // 2. Make sure that path and file names will not overflow static buffer
    unsigned int pstrlen = strlen(ptolemy);
    if ( ( sizeof(SH_ITCLTK_PROG) + pstrlen >= MAX_PATH_LENGTH ) ||
         ( sizeof(SH_ITCLTK_FILE) + pstrlen >= MAX_PATH_LENGTH ) ) {
      abortHandling();
    }

    // 3. Define the path of itcl_wish and itcl file name to source
    strcpy(path, ptolemy);
    strcat(path, SH_ITCLTK_PROG);
    strcpy(file, ptolemy);
    strcat(file, SH_ITCLTK_FILE);

    switch(fork()) 
    {
        case -1:		// fork() return value for error. 
	    abortHandling();

	case 0:			// fork() return value for child. 
	    sleep(1);		// Allow time for core file to be generated.   
	    execle(path, "itcl_wish", file, "-name", "Tycho", (char *)0, \
		   environ);
	    abortHandling();	// If you make it this far something went wrong
    }

    // We are now in the parent process.  Kill off parent.
    exit(0);
}


// signalHandlerDebug
//
// This function defines the way that a signal that is received will
// be handled when PT_DEVELOP is defined to be non-zero.  The action
// of this function is to inform the user that a fatal error occured,
// then call the debugger, if requested, to view the core file.

void signalHandlerDebug(int signo)
{
    // We check this in case the call to DoTychoSave bombs 
    // out and generates another signal. 
    static int times = 0;
    if (times == 0) {
        times++;
	DoTychoSave();
    }

    // Use execle to execute the Unix command "$path $file -name Tycho" where
    //   path = $PTOLEMY/bin.$PTARCH/itkwish
    //   file = $PTOLEMY/tycho/kernel/TyCoreRelease.itcl
    // Cannot use dynamic memory
    static char path[MAX_PATH_LENGTH], file[MAX_PATH_LENGTH];

    // 1. Find the value of the PTOLEMY environment variable
    const char *ptolemy = getenv("PTOLEMY");
    if ( ptolemy == 0 ) ptolemy = "~ptolemy";

    // 2. Make sure that path and file names will not overflow static buffer
    unsigned int pstrlen = strlen(ptolemy);
    if ( ( sizeof(SH_ITCLTK_PROG) + pstrlen >= MAX_PATH_LENGTH ) ||
         ( sizeof(SH_ITCLTK_FILE) + pstrlen >= MAX_PATH_LENGTH ) ) {
      abortHandling();
    }

    // 3. Define the path of itcl_wish and itcl file name to source
    strcpy(path, ptolemy);
    strcat(path, SH_ITCLTK_PROG);
    strcpy(file, ptolemy);
    strcat(file, SH_ITCLTK_FILE);

    switch(fork()) 
    {
        case -1:		// fork() return value for error. 
	    abortHandling();
      
	case 0:			// fork() return value for child. 
	    sleep(1);		// Allow core file to be generated.  
	    execle(path, "itcl_wish", file, "-name", "Tycho", (char *)0, \
		   environ);
	    abortHandling();	// If you make it this far something went wrong
    }

    // We are now in the parent process
    ptSignal(signo, (SIG_PF) SIG_DFL);

    // Commit suicide in manner that generates core file for child,
    // if suicide fails abort.               
    if (kill(getpid(), signo) != 0)
        abortHandling();

    // Program should never get to this point. 
}


// abortHandling
//
// This function is called when everything else goes wrong and the
// program has no choice but to bomb out in an ungraceful manner.

void abortHandling() 
{
    static char msg[128] = "\n\nFatal error occured. Tycho was not able to intercept the error signal and deal with it appropriately.\n";

    // Use write instead of printf because it is reentrant.
    // Send message to the standard output
    write(1, msg, sizeof(msg));
    exit(1);
}
