static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	7 February 1992

*/

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
    numIterations = 0;
    stopTime = 1;
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

// Set up the schedule.  Returns TRUE upon success.
int MTDFScheduler::setup(Galaxy& galaxy)
{
    GalStarIter starIter(galaxy);
    MTDFStar* star;

    // Prepare Stars for scheduling.
    prepareForScheduling(galaxy);

    // Initialize Galaxy and all Stars by recursively invoking Star::start().
    galaxy.initialize();

    // Delete any previous Threads.
    deleteThreads();

    // Create Threads for all the Stars.
    createThreads(galaxy);

    return !haltRequested();
}

// Run the simulation.
int MTDFScheduler::run(Galaxy& galaxy)
{
    numIterations = 0;

    // Lower the priority so that the Threads can run.
    main.setPriority(minPriority);

    while((numIterations++ < stopTime) && !haltRequested())
    {
	// Notify all source Threads to start.
	{
	    CriticalSection x(monitor);
	    start.notifyAll();
	}
    }

    // Raise priority again to prevent Threads from running.
    main.setPriority(maxPriority);

    return !haltRequested();
}

// Prepare stars for scheduling.
void MTDFScheduler::prepareForScheduling(Galaxy& galaxy)
{
    GalStarIter starIter(galaxy);
    MTDFStar* star;

    while((star = (MTDFStar*)starIter++) != NULL)
    {
	star->prepareForScheduling();
    }
}

// Create threads and build ThreadList.
void MTDFScheduler::createThreads(Galaxy& galaxy)
{
    GalStarIter starIter(galaxy);
    MTDFStar* star;

    // Create Threads for all the Stars.
    while((star = (MTDFStar*)starIter++) != NULL)
    {
	MTDFThread* t;
	if(star->isItSource())
	{
	    LOG_NEW; t = new MTDFThread(maxPriority, (void(*)(void*,void*))sourceThread, this, star);
	    starThreads.put(t);
	}
	else
	{
	    LOG_NEW; t = new MTDFThread(maxPriority, (void(*)(void*))starThread, star);
	    starThreads.put(t);
	}
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
void MTDFScheduler::setStopTime(float limit)
{
    stopTime = (unsigned int)(limit / schedulePeriod);
}

float MTDFScheduler::getStopTime()
{
    return stopTime * schedulePeriod;
}

// Thread for normal Stars.
void MTDFScheduler::starThread(MTDFStar* star)
{
    // Fire the Star ad infinitum.
    while(TRUE)	star->fire();
}

// Thread for source Stars.
void MTDFScheduler::sourceThread(MTDFScheduler* sched, MTDFStar* star)
{
    // Wait for notification from the Scheduler, then fire the Star.
    while(TRUE)
    {
	{
	    CriticalSection x(sched->monitor);
	    sched->start.wait();
	}
	star->fire();
    }
}
