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
    Author:	T.M. Parks
    Created:	7 January 1993
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "RTDFScheduler.h"
#include "MTDFStar.h"

// Run (or continue) the simulation.
int RTDFScheduler::run()
{
    // Force current time to be startTime.
    {
	CriticalSection x(monitor);
	clock.reset();
    }

    // Lower priority to allow Threads to run.
    thread->setPriority(MTDFThread::minPriority());

    // Wait until enough time has elapsed.
    while( (now() < getStopTime()) && !SimControl::haltRequested());

    // Raise priority to prevent Threads from running.
    thread->setPriority(MTDFThread::maxPriority());

    // Update startTime but keep current time consistent.
    {
	CriticalSection x(monitor);
	startTime += clock.elapsedTime();
	clock.reset();
    }

    StringList msg;
    if (SimControl::haltRequested()) msg << "Run halted ";
    else msg << "Run completed ";
    msg << "at " << now() << '\n';
    msg << "stopTime = " << getStopTime();

    Error::message(msg);

    return !SimControl::haltRequested();
}

// Select thread function for a star.
void (*RTDFScheduler::selectThread(MTDFStar* star))(MTDFStar*)
{
    if (star->period > 0.0) return periodicThread;
    else if (star->isSource()) return sourceThread;
    else return starThread;
}

// Thread for periodic Stars.
void RTDFScheduler::periodicThread(MTDFStar* star)
{
    MTDFScheduler& sched = *(MTDFScheduler*)star->scheduler();
    TimeVal wake = star->lag;
    
    do
    {
	star->thread().sleep(sched.delay(wake));
	wake += star->period;
    } while (star->run());
}

// Thread for (non-periodic) source Stars.
void RTDFScheduler::sourceThread(MTDFStar* star)
{
    RTDFScheduler& sched = *(RTDFScheduler*)star->scheduler();

    while( (sched.now() < sched.getStopTime()) && star->run() );
}

// Set current time.
void RTDFScheduler::setCurrentTime(double time)
{
    CriticalSection x(monitor);
    startTime = time;
    clock.reset();
}

// Current time.
double RTDFScheduler::now()
{
    CriticalSection x(monitor);
    TimeVal t = startTime + clock.elapsedTime();
    return t;
}

// Delay used for sleeping Threads.
TimeVal RTDFScheduler::delay(TimeVal when)
{
    CriticalSection x(monitor);
    return when - (startTime + clock.elapsedTime());
}
