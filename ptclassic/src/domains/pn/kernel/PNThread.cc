/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	18 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFThread.h"
#include "MTDFStar.h"
#include "MTDFScheduler.h"
#include "CriticalSection.h"

#ifdef __GNUG__
extern "C"
{
    int lwp_setpri(thread_t, int);
}
#endif

// Thread for normal Stars.
void starThread(MTDFStar* star)
{
    // Fire the Star ad infinitum.
    while(TRUE)	star->run();
}

// Thread for source Stars.
void sourceThread(MTDFStar* star)
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

// Constructor.
MTDFThread::MTDFThread(unsigned int priority, MTDFStar* star)
    : Thread(priority, (star->numInputs() == 0) ? (void(*)(void*))sourceThread : (void(*)(void*))starThread, star)
{}

// Change the Thread's priority.
int MTDFThread::setPriority(unsigned int priority)
{
    return lwp_setpri(*this, priority);
}
