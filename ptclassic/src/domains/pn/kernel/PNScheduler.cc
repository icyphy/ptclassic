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

#include "PNScheduler.h"
#include "DataFlowStar.h"
#include "PNThread.h"
#include "PNCondition.h"
#include "GalIter.h"

extern const char PNdomainName[];

// Constructor.
PNScheduler::PNScheduler() : monitor()
{
    start = 0;
    schedulePeriod = 1.0;
    setStopTime(0.0);
    setCurrentTime(0.0);
}

// Destructor.
PNScheduler::~PNScheduler()
{
    LOG_DEL; delete start;
}

// Domain identification.
const char* PNScheduler::domain() const
{
    return PNdomainName;
}

// Initialization.
void PNScheduler::setup()
{
    if (galaxy() == NULL)
    {
	Error::abortRun(domain(), "Scheduler has no galaxy.");
	return;
    }

    // Any threads from the previous run that blocked on start
    // will be deleted at this point.
    LOG_DEL; delete start;
    LOG_NEW; start = new PNCondition(monitor);

    galaxy()->initialize();

    // Enable locking on all portholes.
    GalStarIter starIter(*galaxy());
    Star* star;
    while ((star = starIter++) != NULL)
    {
        BlockPortIter portIter(*star);
        PortHole* port;
        while ((port = portIter++) != NULL)
        {
            // Any threads from the previous run that are blocked
            // will be deleted at this point.
            port->disableLocking();
            port->enableLocking(monitor);
        }
    }

    PNThread::setup();
    createThreads();
    setCurrentTime(0.0);
}

// Run (or continue) the simulation.
int PNScheduler::run()
{
    if (SimControl::haltRequested())
    {
	Error::abortRun(*galaxy(), " cannot continue.");
	return FALSE;
    }

    // Allow threads to start.
    PNThread::go();

    while((currentTime < stopTime) && !SimControl::haltRequested())
    {
	PNThread::stop();
	// Notify all source threads to start.
	{
	    CriticalSection region(start->monitor());
	    start->notifyAll();
	}
	PNThread::go();
	currentTime += schedulePeriod;
    }

    // Prevent threads from running.
    PNThread::stop();

    return !SimControl::haltRequested();
}

// Create threads (Kahn Processes).
void PNScheduler::createThreads()
{
    GalStarIter starIter(*galaxy());
    DataFlowStar* star;
    PNThread* t;

    // Create Threads for all the Stars.
    while((star = (DataFlowStar*)starIter++) != NULL)
    {
        if (star->isSource())
	{
	    LOG_NEW; t = new SyncDataFlowProcess(*star, *start);
	}
	else
	{
	    LOG_NEW; t = new DataFlowProcess(*star);
	}
    }
}

// Get the stopping time.
double PNScheduler::getStopTime()
{
    return stopTime;
}

// Set the stopping time.
void PNScheduler::setStopTime(double limit)
{
    stopTime = limit;
}

// Set the stopping time when inside a Wormhole.
void PNScheduler::resetStopTime(double limit)
{
    // Allow Scheduler to run for one schedule period.
    stopTime = limit;
    currentTime = stopTime - schedulePeriod;
}

double PNScheduler::delay(double when)
{
    return 0.0;
}
