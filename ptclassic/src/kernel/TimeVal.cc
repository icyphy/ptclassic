/* 
Copyright (c) 1990-1994 The Regents of the University of California.
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

/* TV_USEC_TYPE and TV_SEC_TYPE are defined in compat.h, but we
 * include these defs for robustness
 */
#ifndef TV_USEC_TYPE
#define TV_USEC_TYPE	long int
#endif

#ifndef TV_SEC_TYPE
#define TV_SEC_TYPE	long int
#endif

TimeVal::TimeVal()
{
    tv_sec = tv_usec = 0;
}

TimeVal::TimeVal(double seconds)
{
    tv_sec = (TV_SEC_TYPE)seconds;
    tv_usec = (TV_USEC_TYPE)(1.0e6 * (seconds - (double)tv_sec));
    normalize();
}

void TimeVal::normalize()
{
    while (tv_usec < 0)
    {
	tv_sec--;
	tv_usec += 1000000;
    }
    while (tv_usec >= 1000000)
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
    TimeVal x;
    x.tv_sec = tv_sec + t.tv_sec;
    x.tv_usec = tv_usec + t.tv_usec;
    x.normalize();
    return x;
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
    TimeVal x;
    x.tv_sec = tv_sec - t.tv_sec;
    x.tv_usec = tv_usec - t.tv_usec;
    x.normalize();
    return x;
}

TimeVal& TimeVal::operator-=(const TimeVal& t)
{
    tv_sec -= t.tv_sec;
    tv_usec -= t.tv_usec;
    normalize();
    return *this;
}

int TimeVal::operator>(const TimeVal& t) const
{
    return timercmp(this, &t, >);
}

int TimeVal::operator<(const TimeVal& t) const
{
    return timercmp(this, &t, <);
}
