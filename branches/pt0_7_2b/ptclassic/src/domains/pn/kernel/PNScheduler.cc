/* 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*  Version @(#)PNScheduler.cc	2.12 3/7/96
    Author:	T.M. Parks
    Created:	7 February 1992
*/

static const char file_id[] = "PNScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PNThread.h"
#include "PNScheduler.h"
#include "DataFlowStar.h"
#include "GalIter.h"
#include "PNGeodesic.h"

extern const char PNdomainName[];

// Constructor.
PNScheduler::PNScheduler()
{
    monitor = NULL;
    start = NULL;
    threads = NULL;

    schedulePeriod = 1.0;
    setStopTime(0.0);
    setCurrentTime(0.0);
    iteration = 0;
}


// Destructor.
PNScheduler::~PNScheduler()
{
    deleteThreads();
    disableLocking();
}


// Domain identification.
const char* PNScheduler::domain() const
{
    return PNdomainName;
}


// Initialization.
void PNScheduler::setup()
{
    if (! galaxy())
    {
	Error::abortRun("Process Networks Scheduler has no galaxy.");
	return;
    }
   
    deleteThreads();
    galaxy()->initialize();
    disableLocking();

    setCurrentTime(0.0);
    iteration = 0;
    PNGeodesic::resetFull();

    enableLocking();
    createThreads();
}


// Run (or continue) the simulation.
int PNScheduler::run()
{
    if (SimControl::haltRequested() || ! galaxy()) {
	Error::abortRun("Process Networks Scheduler has no galaxy to run");
	return FALSE;
    }

    while((currentTime < stopTime) && !SimControl::haltRequested())
    {
	// Notify all threads to continue.
	{
	    CriticalSection region(start->monitor());
	    iteration++;
	    start->notifyAll();
	}
	PNThread::runAll();
	while (PNGeodesic::blockedOnFull() > 0 && !SimControl::haltRequested())
	{
	    increaseBuffers();
	    PNThread::runAll();
	}
	currentTime += schedulePeriod;
    }

    return !SimControl::haltRequested();
}

// Increase buffer capacities.
// Return number of full buffers encountered.
int PNScheduler::increaseBuffers()
{
    if (! galaxy()) return 0;

    int fullBuffers = 0;
    PNGeodesic* smallest = NULL;
    
    // Increase the capacity of the smallest full geodesic.
    GalStarIter nextStar(*galaxy());
    Star* star;
    while ((star = nextStar++) != NULL)
    {
        BlockPortIter nextPort(*star);
        PortHole* port;
        while ((port = nextPort++) != NULL)
        {
	    PNGeodesic* geo = NULL;
	    if (port->isItOutput() && (geo = (PNGeodesic*)port->geo()) != NULL)
	    {
		if (geo->size() >= geo->capacity())
		{
		    fullBuffers++;
		    if (smallest == NULL
			|| geo->capacity() < smallest->capacity())
			smallest = geo;
		}
	    }
        }
    }
    if (smallest != NULL)
	smallest->setCapacity(smallest->capacity() + 1);

    return fullBuffers;
}


// Create threads (dataflow pocesses).
void PNScheduler::createThreads()
{
    if (! galaxy()) return;

    GalStarIter nextStar(*galaxy());
    DataFlowStar* star;

    LOG_NEW; threads = new ThreadList;

    // Create Threads for all the Stars.
    while((star = (DataFlowStar*)nextStar++) != NULL)
    {
	LOG_NEW; SyncDataFlowProcess* p
	    = new SyncDataFlowProcess(*star,*start,iteration);
	threads->add(p);
	p->initialize();
    }
}


// Delete threads.
void PNScheduler::deleteThreads()
{
    LOG_DEL; delete threads; threads = NULL;
}


// Enable locking.
void PNScheduler::enableLocking()
{
    if (! galaxy()) return;

    // Create new lock for this scheduler.
    LOG_NEW; monitor = new PNMonitor;
    LOG_NEW; start = new PNCondition(*monitor);

    // Enable all registered PtGates.
    GateKeeper::enableAll(*monitor);

    // Enable locking on all portholes.
    GalStarIter nextStar(*galaxy());
    Star* star;
    while ((star = nextStar++) != NULL)
    {
        BlockPortIter nextPort(*star);
        PortHole* port;
        while ((port = nextPort++) != NULL)
        {
            port->enableLocking(*monitor);
        }
    }
}


// Disable locking.
void PNScheduler::disableLocking()
{
    if (! galaxy()) return;

    // Disable locking on all portholes.
    GalStarIter nextStar(*galaxy());
    Star* star;
    while ((star = nextStar++) != NULL)
    {
        BlockPortIter nextPort(*star);
        PortHole* port;
        while ((port = nextPort++) != NULL)
        {
            port->disableLocking();
        }
    }

    // Disable all registered PtGates.
    GateKeeper::disableAll();

    // Delete the lock for this scheduler.
    LOG_DEL; delete start; start = NULL;
    LOG_DEL; delete monitor; monitor = NULL;
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


double PNScheduler::delay(double /*when*/)
{
    return 0.0;
}
