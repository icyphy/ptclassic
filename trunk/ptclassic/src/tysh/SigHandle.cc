/* 
Copyright (c) 1990-1995 The Regents of the University of California.
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
@(#)SigHandle.cc	

Author: Joel R. King

Defines the signal handlers for Tycho.

*****************************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>		
#include "compat.h"
#include "tcl.h"
#include "ptsignals.h"

/****************************************************************************/

#ifdef PTHPPA
typedef void (*SIG_PF)(int);
#endif

/****************************************************************************/

extern Tcl_Interp *ptkInterp; /* Used so we can call emergencySave. */
extern "C" char **environ;/* An array of pointers to strings containing the */
                       /* environmental variables. This is needed by the    */
                       /* execle() function, as an argument to setup the    */
                       /* environment for the process that its launching.   */

/****************************************************************************/

int setHandlers(SIG_PF sigHandler);
void signalHandlerRelease(int);
void signalHandlerDebug(int);
void abortHandling();
static void DoTychoSave(void);

/****************************************************************************/

/***** This function sets the signal handler function for each of the   *****/
/***** signals that we want to intercept.                               *****/

int 
setHandlers(SIG_PF sigHandler)
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

/****************************************************************************/

/***** This function defines the way that a signal that is received     *****/
/***** will be handled when PT_DEVELOP is not defined, or 0. The action *****/
/***** of this function is to issue an error message letting the user   *****/
/***** know that a fatal error occured, then exit.                      *****/

void 
signalHandlerRelease(void)
{

    /*** FIX ME - Should not guess at size of path. ***/
    char *ptolemy, *arch, path[200], file[200];
    static int times = 0;

    if (times == 0) // We check this in case the call to DoTychoSave bombs 
    {               // out and generates another signal. 
        times++;
	DoTychoSave();
    }

    arch = getenv("ARCH");
    ptolemy = getenv("PTOLEMY");
    path[0] = '\0';
    file[0] = '\0';
    strcat(path, ptolemy);
    strcat(path, "/bin.");
    strcat(path, arch);
    strcat(path, "/itcl_wish");
    strcat(file, ptolemy);
    strcat(file, "/tycho/kernel/TyCoreRelease.itcl");
    
    switch(fork()) 
    {
        case -1: // fork() return value for error. 
	    abortHandling();
      
	case 0: // fork() return value for child. 
	    sleep(1); // Allow core file to be generated.   
	    execle(path, "itcl_wish", file, "-name", "Tycho", (char *)0, \
		   environ);
	    abortHandling(); // If you make it this far something went wrong.
    }
    /*************************** In parent ******************************/

    exit(0); // Kill off parent.

}

/****************************************************************************/

/***** This function defines the way that a signal that is received     *****/
/***** will be handled when PT_DEVELOP is defined to be non-zero. The   *****/
/***** action of this function is to inform the user that a fatal error *****/
/***** occured, then call the debugger, if requested, to view the core  *****/
/***** file.                                                            *****/

void
signalHandlerDebug(int signo)
{

    /*** FIX ME - Should not guess at size of path. ***/
    char *ptolemy, *arch, path[200], file[200];
    static int times = 0;

    if (times == 0) // We check this in case the call to DoTychoSave bombs 
    {               // out and generates another signal. 
        times++;
	DoTychoSave();
    }

    ptolemy = getenv("PTOLEMY");
    arch = getenv("ARCH");
    path[0] = '\0';
    file[0] = '\0';
    strcat(path, ptolemy);
    strcat(path, "/bin.");
    strcat(path, arch);
    strcat(path, "/itcl_wish");
    strcat(file, ptolemy);
    strcat(file, "/tycho/kernel/TyCoreDebug.itcl");

    switch(fork()) 
    {
        case -1: // fork() return value for error. 
	    abortHandling();
      
	case 0: // fork() return value for child. 
	    sleep(1); // Allow core file to be generated.  
	    execle(path, "itcl_wish", file, "-name", "Tycho", (char *)0, \
		   environ);
	    abortHandling(); // If you make it this far something went wrong.
    }

    /************************ In parent **********************************/

    ptSignal(signo, (SIG_PF) SIG_DFL);

    if (kill(getpid(), signo) != 0)  // Commit suicide in manner that      
        abortHandling();            // generates core file for child, if  
                                     // suicide fails abort.               

    // Program should never get to this point. 

}

/****************************************************************************/

/**** This function is called when everything else goes wrong and the    ****/
/**** program has no choice but to bomb out in an ungraceful manner.     ****/

void
abortHandling() 
{

    static char msg[400]="\n\nFatal error occured. Tycho was not able to intercept the error signal and deal with it appropriately.\n";
    write(1, msg, sizeof(msg));  // Used instead of printf,      
                                 // because it is reentrant.     
    exit(1);

}

/****************************************************************************/

/**** This function calls the method emergencySave, which attempts to do ****/
/**** an emergency save of all active files with names. It is a method   ****/
/**** of the class TyConsole and we invoke it for the object             ****/
/**** .mainConsole because we know this object is alive as long as the   ****/
/**** program is.                                                        ****/

static void DoTychoSave(void)
{
    Tcl_VarEval(ptkInterp, ".mainConsole emergencySave", NULL);
}    

/****************************************************************************/
