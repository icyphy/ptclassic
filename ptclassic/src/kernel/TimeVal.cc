/* 
Copyright (c) 1990-1993 The Regents of the University of California.
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
/* Version $Id$
   Author:	T. M. Parks
   Created:	8 Nov 91
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "TimeVal.h"
#include "type.h"
#include <signal.h>
#include <sys/types.h>

extern "C" int select(int width, fd_set* read, fd_set* write, fd_set* except,
			const timeval* timeout);

TimeVal::TimeVal()
{
    tv_sec = tv_usec = 0;
}

TimeVal::TimeVal(long sec, long usec)
{
    tv_sec = sec;
    tv_usec = usec;
    normalize();
}

TimeVal::TimeVal(double seconds)
{
    tv_sec = long(seconds);
    tv_usec = long(1.0e6 * (seconds - (double)tv_sec));
    normalize();
}

void TimeVal::normalize()
{
    while (tv_usec < 0)
    {
	tv_sec--;
	tv_usec += 1000000;
    }
    while (tv_usec > 1000000)
    {
	tv_sec++;
	tv_usec -= 1000000;
    }
}

TimeVal::operator double() const
{
    return ((double)tv_sec + (1.0e-6 * (double)tv_usec ));
}

TimeVal TimeVal::operator+(const TimeVal& t) const
{
    long sec = tv_sec + t.tv_sec;
    long usec = tv_usec + t.tv_usec;
    return TimeVal(sec,usec);
}

TimeVal& TimeVal::operator+=(const TimeVal& t)
{
    tv_sec += t.tv_sec;
    tv_usec += t.tv_usec;
    normalize();
    return *this;
}

TimeVal TimeVal::operator-(const TimeVal& t) const
{
    long sec = tv_sec - t.tv_sec;
    long usec = tv_usec - t.tv_usec;
    return TimeVal(sec, usec);
}

TimeVal& TimeVal::operator-=(const TimeVal& t)
{
    tv_sec -= t.tv_sec;
    tv_usec -= t.tv_usec;
    normalize();
    return *this;
}

// Return TRUE if a pause was required (non-negative delay).
int TimeVal::sleep() const
{
    // sleep only if the delay is non-negative.
    if (tv_sec >= 0 && tv_usec >= 0)
    {
	select(0, NULL, NULL, NULL, this);
	return TRUE;
    }
    else return FALSE;
}
