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
