#ifndef _RTScheduler_h
#define _RTScheduler_h

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
*/
/*  Version $Id$
    Author:	T. M. Parks
    Created:	7 January 1993

    Real-time scheduler for multi-threaded dataflow.
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "PNScheduler.h"
#include "Clock.h"

class RTScheduler : public PNScheduler
{
public:
    // Run (or continue) the simulation.
    int run();

    // Current time.
    /*virtual*/ void setCurrentTime(double);
    double now();

    // Delay used for sleeping Threads.
    /*virtual*/ double delay(double);

protected:
    // Select Thread function for a Star.
    /*virtual*/ void (*selectThread(PNStar*))(PNStar*);

    // Thread functions.
    static void periodicThread(PNStar*);
    static void sourceThread(PNStar*);

private:
    Clock clock;
    TimeVal startTime;
};

#endif
