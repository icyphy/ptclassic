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
    Date of creation:	7 February 1992
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
#include "CriticalSection.h"

extern const char MTDFdomainName[];

const int stackSize = 0x4000;

// Create static Thread for main().
MTDFThread& MTDFScheduler::main
    = (MTDFThread&)Thread::init(MTDFThread::minPriority()+1, stackSize);

// Constructor.
MTDFScheduler::MTDFScheduler() : starThreads(), monitor(), start(monitor)
{
    currentTime = 0.0;
    stopTime = 1.0;
    schedulePeriod = 1.0;
}

// Destructor.
MTDFScheduler::~MTDFScheduler()
{
    deleteThreads();
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
    createThreads();

    currentTime = 0.0;
}

// Run (or continue) the simulation.
int MTDFScheduler::run()
{
    // Lower priority to allow Threads to run.
    main.setPriority(MTDFThread::minPriority());

    while((currentTime < stopTime) && !SimControl::haltRequested())
    {
	// Notify all source Threads to start.
	{
	    CriticalSection x(monitor);
	    start.notifyAll();
	}
	currentTime += schedulePeriod;
    }

    // Raise priority to prevent Threads from running.
    main.setPriority(MTDFThread::maxPriority());

    return !SimControl::haltRequested();
}

// Select thread function for a star.
void (*MTDFScheduler::thread(MTDFStar* star))(MTDFStar*)
{
    if (star->isSource()) return sourceThread;
    else return starThread;
}

// Thread for normal Stars.
void MTDFScheduler::starThread(MTDFStar* star)
{
    // Fire the Star ad infinitum.
    while(TRUE)	star->run();
}

// Thread for source Stars.
void MTDFScheduler::sourceThread(MTDFStar* star)
{
    MTDFScheduler* sched = (MTDFScheduler*) star->scheduler();

    // Wait for notification from the Scheduler, then fire the Star.
    while(TRUE)
    {
	{
	    CriticalSection x(sched->monitor);
	    sched->start.wait();
	}
	star->run();
    }
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
	LOG_NEW; t = new MTDFThread(p, thread(star), star);
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
