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

#include "TimeVal.h"
#include "type.h"
#include <signal.h>

#ifdef __GNUG__
typedef void (*SIG_PF)(int);
#endif

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

// Signal handler used by sleep().
static int gotcha = FALSE;
void alarmHandler(int)
{
    gotcha = TRUE;
}

/* This sleep function makes exclusive use of the real interval timer.
   No effort is made to save or restore its previous state.
   Return TRUE if a pause was required (non-negative delay).
*/
int TimeVal::sleep() const
{
    // sleep only if the delay is non-negative.
    if (tv_sec >= 0 && tv_usec >= 0)
    {
	itimerval i;
	i.it_interval.tv_sec = i.it_interval.tv_usec = 0;
	i.it_value.tv_sec = tv_sec;
	i.it_value.tv_usec = tv_usec;
	SIG_PF act = signal(SIGALRM, (SIG_PF)alarmHandler);
	setitimer(ITIMER_REAL, &i, 0);
	while(!gotcha) sigpause(0);
#	ifndef hpux
	signal(SIGALRM, act);	// restore previous action
#	endif
	gotcha = FALSE;
	return TRUE;
    }
    else return FALSE;
}
