/* 
Rountines for blocking interrupts caused by the arrival of signals
   $Id$
*/
/*
Copyright (c) 1990-1995 The Regents of the University of California.
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


#include <signal.h>
#include "compat.h" 

#if defined(PTSOL2) || defined(PTIRIX5) || defined(PTLINUX)
void ptSafeSig( int SigNum ) {
	struct sigaction pt_alarm_action;
	sigaction( SigNum, NULL, &pt_alarm_action);
        pt_alarm_action.sa_flags |= SA_RESTART;
        sigaction( SigNum, &pt_alarm_action, NULL);
}
void ptBlockSig (int SigNum) {};
void ptReleaseSig (int SigNum) {};

#else 
#if defined(PTHPPA)
void ptSafeSig( int SigNum ) { /* to avoid warning */ int a = SigNum;};
long signalmask;
void ptBlockSig( int SigNum ) {
	signalmask = sigblock(sigmask(SigNum));
}
void ptReleaseSig( int SigNum ) {
	/* remove this signal from the signal mask */
	signalmask &= !(sigmask(SigNum));
	sigsetmask(signalmask);
}

#else
#if defined(PTSUN4)
void ptBlockSig (int SigNum) {};
void ptReleaseSig (int SigNum) {};
void ptSafeSig( int SigNum ) {};

#else
/*default is no assignment*/
void ptBlockSig (int SigNum) {};
void ptReleaseSig (int SigNum) {};
void ptSafeSig( int SigNum ) {};

#endif /* PTSUN4 */
#endif /* PTHPPA */
#endif /* PTSOL2  PTIRIX5  PTLINUX */
