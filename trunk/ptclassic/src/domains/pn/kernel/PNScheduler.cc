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
    Created:	7 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFScheduler.h"
#include "MTDFStar.h"
#include "MTDFThread.h"
#include "MTDFThreadIter.h"
#include "GalIter.h"

extern const char MTDFdomainName[];

// Master PtGate object for mutual exclusion.
LwpMonitor masterMonitor;

// Constructor.
MTDFScheduler::MTDFScheduler() : starThreads(), monitor(), start(monitor)
{
    schedulePeriod = 1.0;
    setStopTime(0.0);
    setCurrentTime(0.0);

    // Initialize the Lightweight Process library.
    const int stackSize = 0x4000;
    MTDFThread::initStackCache(stackSize);
    MTDFThread::setMaxPriority(MTDFThread::minPriority()+1);
    thread = MTDFThread::currentThread();
    thread->setPriority(MTDFThread::maxPriority());

    // Enable all registered PtGates before additional threads are created.
    GateKeeper::enableAll(masterMonitor);
}

// Destructor.
MTDFScheduler::~MTDFScheduler()
{
    deleteThreads();
    LOG_DEL; delete thread;

    // Disable all registered PtGates after other threads have been deleted.
    GateKeeper::disableAll();
}

// Domain identification.
const char* MTDFScheduler::domain() const
{
    return MTDFdomainName;
}

// Initialization.
void MTDFScheduler::setup()
{
    if (galaxy() == NULL)
    {
	Error::abortRun(domain(), "Scheduler has no galaxy.");
	return;
    }

    galaxy()->initialize();

    // Delete any left-over threads before creating new ones.
    deleteThreads();

    if (!SimControl::haltRequested())
	createThreads();

    setCurrentTime(0.0);
}

// Run (or continue) the simulation.
int MTDFScheduler::run()
{
    // Lower priority to allow other threads to run.
    thread->setPriority(MTDFThread::minPriority());

    while((currentTime < stopTime) && !SimControl::haltRequested())
    {
	// Notify all source threads to start.
	{
	    CriticalSection x(monitor);
	    start.notifyAll();
	}
	currentTime += schedulePeriod;
    }

    // Raise priority to prevent other threads from running.
    thread->setPriority(MTDFThread::maxPriority());

    return !SimControl::haltRequested();
}

// Select thread function for a star.
void (*MTDFScheduler::selectThread(MTDFStar* star))(MTDFStar*)
{
    if (star->isSource()) return sourceThread;
    else return starThread;
}

// Thread for normal Stars.
void MTDFScheduler::starThread(MTDFStar* star)
{
    // Fire the Star ad infinitum.
    while(star->run());
}

// Thread for source Stars.
void MTDFScheduler::sourceThread(MTDFStar* star)
{
    MTDFScheduler& sched = *(MTDFScheduler*)star->scheduler();

    // Wait for notification from the Scheduler, then fire the Star.
    do
    {
	CriticalSection x(sched.monitor);
	sched.start.wait();
    } while (star->run());
}

// Create threads and build ThreadList.
void MTDFScheduler::createThreads()
{
    GalStarIter starIter(*galaxy());
    MTDFStar* star;
    MTDFThread* t;
    int p = MTDFThread::maxPriority();

    // Create Threads for all the Stars.
    while((star = (MTDFStar*)starIter++) != NULL)
    {
	LOG_NEW; t = new MTDFThread(p, selectThread(star), star);
	starThreads.append(t);
    }
}

// Delete Threads and clear the ThreadList.
void MTDFScheduler::deleteThreads()
{
    MTDFThreadIter threadIter(starThreads);
    MTDFThread* t;

    // Delete all Threads in the list.
    while((t = threadIter++) != NULL)
    {
	LOG_DEL; delete t;
    }

    // Remove everything from the list.
    starThreads.initialize();
}

// Set the stopping time.
void MTDFScheduler::setStopTime(double limit)
{
    stopTime = limit;
}

double MTDFScheduler::getStopTime()
{
    return stopTime;
}

TimeVal MTDFScheduler::delay(TimeVal when)
{
    TimeVal zero;
    return zero;
}
