/* Version $Id$
   Copyright 1991 The Regents of the University of California.
   All Rights Reserved.

   Programmer:  T. M. Parks
   Date of creation:  8 Nov 91
*/

#ifndef _Clock_h
#define _Clock_h

#ifdef __GNUG__
#pragma interface
#endif

#include "TimeVal.h"

class Clock
{
public:
    Clock();

    // Reset startTime.
    void reset();

    // Elapsed time since last reset.
    TimeVal elapsedTime() const;

    // Sleep until specified elapsed time.
    int sleepUntil(const TimeVal&) const;
private:
    // When the clock was started.
    TimeVal startTime;

    // Read the system clock.
    TimeVal timeOfDay() const;

};

#endif
