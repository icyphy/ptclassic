/* Version $Id$
   Copyright 1991 The Regents of the University of California.
   All Rights Reserved.

   Programmer:  T. M. Parks
   Date of creation:  8 Nov 91
*/

#ifndef _TimeVal_h
#define _TimeVal_h

#ifdef __GNUG__
#pragma interface
#endif

#include <sys/time.h>

class TimeVal : private timeval
{
friend class Clock;

public:
    TimeVal();
    TimeVal(long sec, long usec);
    TimeVal(double);
    operator double() const;

    TimeVal operator +(const TimeVal&) const;
    TimeVal operator -(const TimeVal&) const;
    TimeVal& operator +=(const TimeVal&);
    TimeVal& operator -=(const TimeVal&);

private:
    void normalize();

    // Sleep using current time value as delay.
    // Return TRUE if pause was required (non-negative delay).
    int sleep() const;
};

#endif
