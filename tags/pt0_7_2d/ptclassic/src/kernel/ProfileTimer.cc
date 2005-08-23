/* 
Copyright (c) 1990-1997 The Regents of the University of California.
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

   Version: @(#)ProfileTimer.cc	1.6 12/15/97
   Programmer:  Jose Luis Pino
   Date of creation:  3/27/95

*/

static const char file_id[] = "ProfileTimer.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ProfileTimer.h"
#include <sys/time.h>
#include "compat.h"

#if !defined(PTSOL2_4) && (!defined (__GLIBC__) || (__GLIBC__ < 2))
extern "C" int getitimer(int, struct itimerval *);
#endif

int ProfileTimer::setupFlag = 0;

ProfileTimer::ProfileTimer() {
#if !defined(PT_NO_TIMER)
    if (setupFlag != 0xABCDEF) {
	itimerval year;
	// One year until this interval timer triggers an interrupt.  If 
	// this ptolemy process has been running for a year....  
	// then we will crash.
	year.it_value.tv_sec = 3600 * 24 * 355;
	year.it_interval.tv_usec = 
	    year.it_value.tv_usec = 0;
	// HPUX10.20 CC compiler warning: tv_usec might be an int, not a long 
        // Message was: warning: long assigned to int (276)
	year.it_interval.tv_sec = 0 ;

	setitimer(ITIMER_PROF,&year,0);
	setupFlag = 0xABCDEF;
    }
#endif
    reset();
}

// Reset startTime.
void ProfileTimer::reset() {
    startTime = timeOfProfile();
}

// Elapsed time in seconds since last reset.
TimeVal ProfileTimer::elapsedCPUTime() const {
    // time is counting down
    return startTime - timeOfProfile();
}

// Read the system clock.
TimeVal ProfileTimer::timeOfProfile() const {
#if defined(PT_NO_TIMER)
    return 0;
#else
    itimerval time;
    getitimer(ITIMER_PROF,&time);
    TimeVal t = time.it_value;
    return t;
#endif  //PT_NO_TIMER
}
