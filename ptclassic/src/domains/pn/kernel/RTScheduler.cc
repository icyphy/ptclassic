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

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "RTScheduler.h"
#include "PNStar.h"

// Run (or continue) the simulation.
int RTScheduler::run()
{
    // Force current time to be startTime.
    {
	CriticalSection x(monitor);
	clock.reset();
    }

    // Lower priority to allow Threads to run.
    thread->setPriority(PNThread::minPriority());

    // Wait until enough time has elapsed.
    while( (now() < getStopTime()) && !SimControl::haltRequested());

    // Raise priority to prevent Threads from running.
    thread->setPriority(PNThread::maxPriority());

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
void (*RTScheduler::selectThread(PNStar* star))(PNStar*)
{
    if (star->period > 0.0) return periodicThread;
    else if (star->isSource()) return sourceThread;
    else return starThread;
}

// Thread for periodic Stars.
void RTScheduler::periodicThread(PNStar* star)
{
    PNScheduler& sched = *(PNScheduler*)star->scheduler();
    double wake = star->lag;
    
    do
    {
	star->thread().sleep(sched.delay(wake));
	wake += star->period;
    } while (star->run());
}

// Thread for (non-periodic) source Stars.
void RTScheduler::sourceThread(PNStar* star)
{
    RTScheduler& sched = *(RTScheduler*)star->scheduler();

    while( (sched.now() < sched.getStopTime()) && star->run() );
}

// Set current time.
void RTScheduler::setCurrentTime(double time)
{
    CriticalSection x(monitor);
    startTime = time;
    clock.reset();
}

// Current time.
double RTScheduler::now()
{
    CriticalSection x(monitor);
    TimeVal t = startTime + clock.elapsedTime();
    return t;
}

// Delay used for sleeping Threads.
double RTScheduler::delay(double when)
{
    CriticalSection x(monitor);
    return when - now();
}
