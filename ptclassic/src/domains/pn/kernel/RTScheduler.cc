/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
/*  Version $Id$
    Programmer:		T.M. Parks
    Date of creation:	7 January 1993
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "RTDFScheduler.h"
#include "MTDFStar.h"
#include "CriticalSection.h"

// Constructor.
RTDFScheduler::RTDFScheduler()
{
    setCurrentTime(0.0);
}

// Initialization.
void RTDFScheduler::setup()
{
    MTDFScheduler::setup();
    setCurrentTime(0.0);
}

// Run (or continue) the simulation.
int RTDFScheduler::run()
{
    // Force current time to be startTime.
    clock.reset();

    // Lower priority to allow Threads to run.
    main.setPriority(MTDFThread::minPriority());

    // Wait until enough time has elapsed.
    while ( (startTime + clock.elapsedTime() < stopTime)
	    && !SimControl::haltRequested() )
	;

    // Raise priority to prevent Threads from running.
    main.setPriority(MTDFThread::maxPriority());

    // Update startTime but keep current time consistent.
    startTime += clock.elapsedTime();
    clock.reset();

    return !SimControl::haltRequested();
}

// Select thread function for a star.
void (*RTDFScheduler::thread(MTDFStar* star))(MTDFStar*)
{
    if (star->period > 0.0) return periodicThread;
    else return starThread;
}

// Thread for periodic Stars.
void RTDFScheduler::periodicThread(MTDFStar* star)
{
    RTDFScheduler* sched = (RTDFScheduler*)star->scheduler();
    TimeVal wake = star->lag;
    
    while(TRUE)
    {
	star->thread().sleep(wake - sched->now());
	star->run();
	wake += star->period;
    }
}

// Set current time.
void RTDFScheduler::setCurrentTime(double time)
{
    startTime = time;
    clock.reset();
}

// Current time.
double RTDFScheduler::now() const
{
    return startTime + clock.elapsedTime();
}
