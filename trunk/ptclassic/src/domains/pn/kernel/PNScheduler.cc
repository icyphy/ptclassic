/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	7 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFScheduler.h"
#include "MTDFStar.h"
#include "MTDFThreadIter.h"
#include "GalIter.h"
#include "CriticalSection.h"

extern const char MTDFdomainName[];

const int stackSize = 0x4000;
const int minPriority = MINPRIO;
const int maxPriority = MINPRIO+1;

// Create static Thread for main().
MTDFThread& MTDFScheduler::main = (MTDFThread&)Thread::init(maxPriority, stackSize);

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

    currentTime = 0.0;
    galaxy()->initialize();

    // Delete any threads from the last run before creating new ones.
    deleteThreads();
    createThreads();
}

// Run (or continue) the simulation.
int MTDFScheduler::run()
{
    // Lower the priority so that the Threads can run.
    main.setPriority(minPriority);

    while((currentTime < stopTime) && !haltRequested())
    {
	// Notify all source Threads to start.
	{
	    CriticalSection x(monitor);
	    start.notifyAll();
	}
	currentTime += schedulePeriod;
    }

    // Raise priority again to prevent Threads from running.
    main.setPriority(maxPriority);

    return !haltRequested();
}

// Create threads and build ThreadList.
void MTDFScheduler::createThreads()
{
    GalStarIter starIter(*galaxy());
    MTDFStar* star;
    MTDFThread* thread;

    // Create Threads for all the Stars.
    while((star = (MTDFStar*)starIter++) != NULL)
    {
	LOG_NEW; thread = new MTDFThread(maxPriority, star);
	starThreads.append(thread);
    }
}

// Delete Threads and clear the ThreadList.
void MTDFScheduler::deleteThreads()
{
    // Delete all Threads in the list.
    MTDFThreadIter threadIter(starThreads);
    MTDFThread* t;
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
