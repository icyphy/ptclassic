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
#include "InfString.h"

// Define the name of the [incr tk] program: either itcl_wish or itkwish
#define SH_ITCLTK_PROG "itkwish"

// Sub-directory of $PTOLEMY that contains itcl files to source plus file names
#define SH_ITCLTK_DEBUG_SCRIPT "/tycho/kernel/TyCoreDebug.itcl"
#define SH_ITCLTK_RELEASE_SCRIPT "/tycho/kernel/TyCoreRelease.itcl"

// Used so we can call the Tcl procedure emergencySave
extern Tcl_Interp *ptkInterp;

// An array of pointers to strings containing the environment variables
// Needed by the execle() function to setup the process it is launching
extern "C" char **environ;

// Use execle to execute the Unix command "$signalPath $signalScript 
// -name Tycho" where
//   signalPath = $PTOLEMY/bin.$PTARCH/itkwish
//   signalScript = $PTOLEMY/tycho/kernel/TyCoreRelease.itcl, or
//   signalScript = $PTOLEMY/tycho/kernel/TyCoreDebug.itcl
// signalMsg is used by abortHandling() to inform user of the error.
// Cannot use dynamic memory since malloc and new are not reentrant.
static char *signalPath = NULL, *signalScript = NULL, *signalMsg = NULL;

// Store size of signalMsg for use by abortHandling. We do this here 
// since neither sizeof() nor strlen are reentrant.
int signalMsgSize;

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

// setReleaseStrings
//
// This function sets the value of path and script for release mode, 
// since this cannot be done dynamically in the signal handler because 
// new and malloc are not reentrant.

void setReleaseStrings(void) 
{

    // Sub-directory of $PTOLEMY that contains the itcl binary plus binary
    // file name, e.g., "/bin.sol2/itkwish"
    InfString itcl_path = "/bin.";
    const char* ptarch = getenv("PTARCH");
    if (ptarch == 0)
    {
        itcl_path << PTARCH;
    }
    else
    {
        itcl_path << ptarch;
    }  
    itcl_path << "/";
    itcl_path << SH_ITCLTK_PROG;

    // 1. Find the value of the PTOLEMY environment variable
    const char* ptolemy = getenv("PTOLEMY");
    if (ptolemy == 0) {
        ptolemy = "~ptolemy";
    }

    // 2. Define the path of [incr tk] and tcltk script to source
    InfString tempPath = ptolemy;
    tempPath << itcl_path;

    InfString tempScript = ptolemy;
    tempScript << SH_ITCLTK_RELEASE_SCRIPT;

    InfString tempMsg = "\n\nFatal error occured. Tycho was not able to intercept the error signal and deal with it appropriately.\n";

    // These values don't change for the life of the program. So if the 
    // environment changes during execution those changes will not be 
    // reflected until program in launched again.
    signalPath = tempPath.newCopy();
    signalScript = tempScript.newCopy();
    signalMsg = tempMsg.newCopy();
    signalMsgSize = strlen(signalMsg);

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

    if (signalPath == NULL || signalScript == NULL) {
        abortHandling();
    }

    switch(fork()) 
    {
        case -1:		// fork() return value for error. 
	    abortHandling();

	case 0:			// fork() return value for child. 
	    sleep(1);		// Allow time for core file to be generated.   
	    execle(signalPath, SH_ITCLTK_PROG, signalScript, "-name", \
"Tycho", (char *)0, environ);
	    abortHandling();	// If you make it this far something went wrong
    }

    // We are now in the parent process. Kill off parent.
    exit(0);

}

// setDebugStrings
//
// This function sets the value of path and script for release mode, 
// since this cannot be done dynamically in the signal handler because 
// new and malloc are not reentrant.

void setDebugStrings(void) 
{

    // Sub-directory of $PTOLEMY that contains the itcl binary plus binary
    // file name, e.g., "/bin.sol2/itkwish"
    InfString itcl_path = "/bin.";
    const char* ptarch = getenv("PTARCH");
    if (ptarch == 0)
    {
        itcl_path << PTARCH;
    }
    else
    {
        itcl_path << ptarch;
    }  
    itcl_path << "/";
    itcl_path << SH_ITCLTK_PROG;

    // 1. Find the value of the PTOLEMY environment variable
    const char* ptolemy = getenv("PTOLEMY");
    if (ptolemy == 0)
    {
        ptolemy = "~ptolemy";
    }

    // 2. Define the path of itcl_wish and itcl file name to source
    InfString tempPath = ptolemy;
    tempPath << itcl_path;

    InfString tempScript = ptolemy;
    tempScript << SH_ITCLTK_DEBUG_SCRIPT;

    InfString tempMsg = "\n\nFatal error occured. Tycho was not able to intercept the error signal and deal with it appropriately.\n";

    // These values don't change for the life of the program. So if the 
    // environment changes during execution those changes will not be 
    // reflected until program in launched again.
    signalPath = tempPath.newCopy();
    signalScript = tempScript.newCopy();
    signalMsg = tempMsg.newCopy();
    signalMsgSize = strlen(signalMsg);

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

    if (signalPath == NULL || signalScript == NULL) {
        abortHandling();
    }

    switch(fork()) 
    {
        case -1:		// fork() return value for error. 
	    abortHandling();
      
	case 0:			// fork() return value for child. 
	    sleep(1);		// Allow core file to be generated.  
	    execle(signalPath, SH_ITCLTK_PROG, signalScript, "-name", \
"Tycho", (char *)0, environ);
	    abortHandling();	// If you make it this far something went wrong
    }

    // We are now in the parent process
    // Set the handler for the current signal to the default, so that  
    // the kernel will generate a core file.
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
    if (signalMsg == NULL) {
        exit(1);
    }
    // Use write instead of printf because it is reentrant.
    // Send message to the standard output
    write(1, signalMsg, signalMsgSize);
    exit(1);
}
