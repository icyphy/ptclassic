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

#ifndef _MTDFScheduler_h
#define _MTDFScheduler_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"
#include "MTDFThreadList.h"
#include "LwpMonitor.h"
#include "TimeVal.h"

class MTDFStar;
class MTDFThread;

class MTDFScheduler : public Scheduler
{
public:
    // Constructor.
    MTDFScheduler();

    // Destructor.
    ~MTDFScheduler();

    // Domain identification.
    /*virtual*/ const char* domain() const;

    // Initialization.
    /*virtual*/ void setup();

    // Run the simulation.
    /*virtual*/ int run();

    // Set the stopping time.
    /*virtual*/ void setStopTime(double);

    // Get the stopping time.
    /*virtual*/ double getStopTime();

    // Create threads and build ThreadList.
    void createThreads();

    // Delete Threads and clear the ThreadList.
    void deleteThreads();

    // Duration of one schedule period or iteration.
    TimeVal schedulePeriod;

    // Delay used for sleeping Threads.
    virtual TimeVal delay(TimeVal);

protected:
    // Stopping time.
    TimeVal stopTime;

    // Thread in which this scheduler is running.
    MTDFThread* thread;

    // List of Star Threads.
    MTDFThreadList starThreads;

    // Select thread function for a star.
    virtual void (*selectThread(MTDFStar*))(MTDFStar*);

    // Thread functions.
    static void starThread(MTDFStar*);
    static void sourceThread(MTDFStar*);

    // Monitor for guarding the Conditions used by the Scheduler.
    LwpMonitor monitor;

    // Condition variable for synchronizing the start of an iteration.
    LwpCondition start;
};

#endif
