/* Version $Id$
   Copyright 1991 The Regents of the University of California.
   All Rights Reserved.

   Programmer:  T. M. Parks
   Date of creation:  8 Nov 91
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Clock.h"

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
    gettimeofday((timeval*)&t,0);
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
    TimeVal delay = time - elapsedTime();
    return delay.sleep();
}
