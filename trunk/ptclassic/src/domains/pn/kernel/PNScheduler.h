/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	7 February 1992
*/

#ifndef _MTDFScheduler_h
#define _MTDFScheduler_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "MTDFThread.h"
#include "MTDFThreadList.h"
#include "Monitor.h"
#include "Condition.h"

class Galaxy;
class MTDFStar;

class MTDFScheduler : public Scheduler
{
    friend void sourceThread(MTDFStar*);

public:
    // Constructor.
    MTDFScheduler();

    // Destructor.
    ~MTDFScheduler();

    // Domain identification.
    /* virtual */ const char* domain() const;

    // Initialization.
    /* virtual */ void setup();

    // Run the simulation.
    /* virtual */ int run();

    // Set the stopping time.
    /* virtual */ void setStopTime(double);

    // Get the stopping time.
    /* virtual */ double getStopTime();

    // Create threads and build ThreadList.
    void createThreads();

    // Delete Threads and clear the ThreadList.
    void deleteThreads();

    // Duration of one schedule period or iteration.
    double schedulePeriod;

protected:
    // Stopping time.
    double stopTime;

    // Thread for main().
    static MTDFThread& main;

    // List of Star Threads.
    MTDFThreadList starThreads;

    // Monitor for guarding the Conditions used by the Scheduler.
    Monitor monitor;

    // Condition variable for synchronizing the start of an iteration.
    Condition start;
};

#endif
