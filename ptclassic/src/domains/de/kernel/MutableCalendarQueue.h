#ifndef _MutableCalendarQueue_h
#define _MutableCalendarQueue_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1997 The Regents of the University of California.
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

 Programmer: John Davis
 Date: 5/19/97
	This class is extended from CalendarQueue. We make this
	extension rather than adding the functionality directly
	to the CalendarQueue class so that the user can easily
	switch between the "regular" CalendarQueue and the
	MutableCalendarQueue.

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include <math.h>
#ifdef sgi
extern const double __infinity;
#define HUGE_VAL __infinity
#endif

#ifndef HUGE_VAL
// Some users report that HUGE_VAL is not defined for them.
// (sun4, gcc-2.4.5, gcc-2.5.8)  There could be nasty problems here.
// linux had problems with the statistics demo and a SIGFPE.
#define HUGE_VAL MAXDOUBLE
#endif


class MutableCalendarQueue : public CalendarQueue
{
public:
	// Remove a CqLevelLink from the CalendarQueue
	int removeEvent( CqLevelLink * cqLevelLink );

	// Override NextEvent()
	CqLevelLink * NextEvent();

private:

}


