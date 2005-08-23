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
*/
/* Version: @(#)Clock.cc	1.18 08/25/97
   Programmer:  T. M. Parks
   Date of creation:  8 Nov 91
*/

static const char file_id[] = "Clock.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Clock.h"
#include "Error.h"
#include "type.h"
#define NEED_TIMEVAL //compat.h reads this
#include "compat.h"


#if !defined(PTSOL2_4) && !defined(PTSOL2_5) && !defined(PTHPUX10)
extern "C" int gettimeofday(timeval *, struct timezone *);
#endif

#if defined(PTHPPA) && !defined(PTHPUX10)
extern "C" int select(size_t, int*, int*, int*, const timeval*);
#else
#include <sys/types.h>
#if defined(__GNUG__) && !defined(PTAIX)
extern "C" int select(int, fd_set*, fd_set*, fd_set*, timeval*);
#endif
#endif

Clock::Clock()
{
    reset();
}

// Reset startTime.
void Clock::reset()
{
    startTime = timeOfDay();
}

// Read the system clock.
TimeVal Clock::timeOfDay() const
{
    TimeVal t;

#ifdef PT_NO_GETTIMEOFDAY
    Error::abortRun("Clock::timeOfDay: gettimeofday() not present at compile time");
#else
    gettimeofday(&t,0);
#endif
    return t;
}

// Elapsed time since last reset.
TimeVal Clock::elapsedTime() const
{
    return timeOfDay() - startTime;
}

// Sleep until specified elapsed time.
int Clock::sleepUntil(const TimeVal& time) const
{
#ifdef PT_NO_SELECT
    Error::abortRun("Clock::sleepUntil(): select() not present at compile time");
#else
    TimeVal elapsed = elapsedTime();
    if (time < elapsed) return FALSE;	// Sleep only if not running late.
    while (time > elapsed)		// select() could return early.
    {
	TimeVal delay = time - elapsed;
	select(0, NULL, NULL, NULL, &delay);
	elapsed = elapsedTime();
    }
#endif
    return TRUE;
}
