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
public:
    // Constructor.
    MTDFScheduler();

    // Destructor.
    ~MTDFScheduler();

    // Domain identification.
    virtual const char* domain() const;

    // Set up the schedule and initialize all Blocks.
    // Return TRUE on success.
    virtual int setup(Galaxy& galaxy);

    // Run the simulation.
    virtual int run(Galaxy& galaxy);

    // Set the stopping time.
    virtual void setStopTime(float limit);

    // Get the stopping time.
    virtual float getStopTime();

    // Prepare stars for scheduling.
    void prepareForScheduling(Galaxy& galaxy);

    // Create threads and build ThreadList.
    void createThreads(Galaxy& galaxy);

    // Delete Threads and clear the ThreadList.
    void deleteThreads();

protected:
    // Number of iterations completed.
    unsigned int numIterations;

    // Stopping time.
    unsigned int stopTime;

    // Duration of one schedule period or iteration.
    float schedulePeriod;

    // Thread for normal Stars.
    static void starThread(MTDFStar* star);

    // Thread for source Stars.
    static void sourceThread(MTDFScheduler* sched, MTDFStar* star);

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
