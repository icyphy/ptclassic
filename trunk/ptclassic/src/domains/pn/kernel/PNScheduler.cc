/* 
Copyright (c) 1990-1994 The Regents of the University of California.
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
#include "MTDFCondition.h"
#include "GalIter.h"

extern const char MTDFdomainName[];

// Constructor.
MTDFScheduler::MTDFScheduler() : monitor()
{
    start = 0;
    schedulePeriod = 1.0;
    setStopTime(0.0);
    setCurrentTime(0.0);
}

// Destructor.
MTDFScheduler::~MTDFScheduler()
{
    LOG_DEL; delete start;
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

    // Any threads from the previous run that blocked on start
    // will be deleted at this point.
    LOG_DEL; delete start;
    LOG_NEW; start = new MTDFCondition(monitor);

    // Any threads from the previous run that are blocked on notEmpty
    // will be deleted at this point.
    galaxy()->initialize();

    MTDFThread::setup();
    createThreads();
    setCurrentTime(0.0);
}

// Run (or continue) the simulation.
int MTDFScheduler::run()
{
    if (SimControl::haltRequested())
    {
	Error::abortRun(*galaxy(), " cannot continue.");
	return FALSE;
    }

    // Allow threads to start.
    MTDFThread::go();

    while((currentTime < stopTime) && !SimControl::haltRequested())
    {
	MTDFThread::stop();
	// Notify all source threads to start.
	{
	    CriticalSection region(start->monitor());
	    start->notifyAll();
	}
	MTDFThread::go();
	currentTime += schedulePeriod;
    }

    // Prevent threads from running.
    MTDFThread::stop();

    return !SimControl::haltRequested();
}

// Create threads and build ThreadList.
void MTDFScheduler::createThreads()
{
    GalStarIter starIter(*galaxy());
    MTDFStar* star;
    MTDFThread* t;

    // Create Threads for all the Stars.
    while((star = (MTDFStar*)starIter++) != NULL)
    {
        if (star->isSource())
	{
	    LOG_NEW; t = new MTDFSourceThread(*star, *start);
	}
	else
	{
	    LOG_NEW; t = new MTDFThread(*star);
	}
    }
}

// Get the stopping time.
double MTDFScheduler::getStopTime()
{
    return stopTime;
}

// Set the stopping time.
void MTDFScheduler::setStopTime(double limit)
{
    stopTime = limit;
}

// Set the stopping time when inside a Wormhole.
void MTDFScheduler::resetStopTime(double limit)
{
    // Allow Scheduler to run for one schedule period.
    stopTime = limit;
    currentTime = stopTime - schedulePeriod;
}

double MTDFScheduler::delay(double when)
{
    return 0.0;
}
