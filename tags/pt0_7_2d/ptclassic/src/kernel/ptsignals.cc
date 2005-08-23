/* 
Rountines for blocking interrupts caused by the arrival of signals
   @(#)ptsignals.cc	1.14 02/04/99
*/
/*
Copyright (c) 1990-1999 The Regents of the University of California.
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
 Programmer: Alan Kamas
*/

/*
These signal rountines are for handling signals that may interrupt calls
such as reads or pauses.  Since this varies for different architectures
some of these routines are not needed while others are.  If the signals 
on an architecture do not interrupt system calls, then none of these
funcitons are needed (and they are thus declared as blank functions).
If the signals do interrupt system calls, but it is possible to turn
this interrupting behaviour off, then ptSafeSig is defined.  Finally,
if it is not possible turn interrupting off, then the blocking must
be used to protect system calls from interrupts.

ptSafeSig: 
    tells the system that this signal is not to interrupt system
    calls.  Any system call that is running with the signal arrives
    is continued as if nothing had happened.  If this function can
    be defined, then holding and releasing are not necessary.

ptBlockSig: 
    Blocks the passed signal.  If the signal comes while the block is
    on, the signal is held and nothing is interrupted.  The signal 
    does not take effect until it is released.

ptReleaseSig:
    Releases the passed signal.  If the signal came while it was 
    blocked, it is now released.

*/

/*

How to add a new architecture to this file
------------------------------------------
First check if signals calls on your architecture will interrupt and
stop system calls.  One way to test this is to build Ptolemy with the
default (do nothing) functions below.  If Ptolemy runs without problems,
then you're done.  If you get errors like: 
     RPC Error: fread of long failed
     RPC Error: application: ../../../../src/octtools/Xpackages/rpc/rpc.c 
                (line 76): function_and_args: Interrupted system call
Then you sould do one of the following below.

If you are running a POSIX machine, then it is quite likely that the
default will not work for you.  And if you get errors like the above
then the default is definitely not working for you.  If this is the
case, check the man page for sigaction.  If the flag SA_RESTART is
defined, then define ptSafeSig as below, and define ptBlockSig
and ptReleaseSig as blank funcitons.

If SA_RESTART is not defined on your system, then define ptSafeSig
as a blank function and define ptBlockSig and ptReleaseSig as below.

    - Alan Kamas 3/95
 
*/


#include <stdio.h>		// Pick up NULL?  freebsd wants this.
#include <signal.h>
#include "compat.h" 
#include "ptsignals.h"

// When the POSIX thread library libgthreads is used,
//  sigaction() must be used instead of signal() to register handlers.
#if defined(PTSOL2) || defined(PTSUN4)

extern "C" SIG_PT ptSignal(int sig, SIG_PT handler)
{
    struct sigaction action;

    sigaction(sig, NULL, &action);

    // These casts are UGLY, but what else are you going to do?!
    SIG_PT old = (SIG_PT)action.sa_handler;
#if defined(PTSOL2_5) && defined(PTSOL2_CFRONT)   
    // See /usr/include/sys/signal.h under solaris2.5 for a complete discussion
    action.sa_handler = (void (*)(int))handler;
#else
    action.sa_handler = (void (*)())handler;
#endif
    sigaction(sig, &action, NULL);
    return old;
}

#endif /*PTSOL2 PTSUN4*/

#if defined(PTSOL2) || defined(PTIRIX5) || defined(PTLINUX) || defined(PTALPHA) || defined(PTAIX) || defined(SA_RESTART)
void ptSafeSig( int SigNum ) {
	struct sigaction pt_alarm_action;
	sigaction( SigNum, NULL, &pt_alarm_action);
        pt_alarm_action.sa_flags |= SA_RESTART;
        sigaction( SigNum, &pt_alarm_action, NULL);
}
void ptBlockSig (int) {};
void ptReleaseSig (int) {};

#else 
#if defined(PTHPPA)
void ptSafeSig( int ) {}
long signalmask = 0;
void ptBlockSig( int SigNum ) {
 	signalmask = sigblock(sigmask(SigNum)) | sigmask(SigNum);
}
void ptReleaseSig( int SigNum ) {
	/* remove this signal from the signal mask */
 	signalmask &= ~(sigmask(SigNum));
	sigsetmask(signalmask);
}

#else
#if defined (PTNT) && !defined(PT_NT4VC)
void ptSafeSig( int ) {}
sigset_t signalmask = 0;
void ptBlockSig( int SigNum ) {
        sigaddset(&signalmask, SigNum);
        sigprocmask(SIG_BLOCK, &signalmask, NULL);

}
void ptReleaseSig( int SigNum ) {
    /* remove this signal from the signal mask */
        sigdelset(&signalmask, SigNum);
        sigprocmask(SIG_UNBLOCK, &signalmask, NULL);
}
#else
#if defined(PTSUN4)
void ptBlockSig(int) {};
void ptReleaseSig(int) {};
void ptSafeSig(int) {};

#else
/*default is no assignment*/
void ptBlockSig (int SigNum) {};
void ptReleaseSig (int SigNum) {};
void ptSafeSig( int SigNum ) {};

#endif /* PTSUN4 */
#endif /* PTNT */
#endif /* PTHPPA */
#endif /* PTSOL2  PTIRIX5  PTLINUX PTALPHA PTAIX SA_RESTART */
