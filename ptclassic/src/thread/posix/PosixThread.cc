static const char file_id[] = "$RCSfile$";

/* 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
/* Version $Id$
   Author:	T. M. Parks
   Created:	21 October 1994
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "PosixThread.h"
#include "DataStruct.h"


// Constructor for new threads.
PosixThread::PosixThread(ThreadScheduler& s) : PtThread(s)
{
    // Initialize attributes.
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    // Detached threads free up their resources as soon as they exit.
    // Non-detached threads can be joined.
    int detach = 0;
    pthread_attr_setdetachstate(&attributes, &detach);

    // New threads inherit their priority and scheduling policy
    // from the current thread.
    pthread_attr_setinheritsched(&attributes, PTHREAD_INHERIT_SCHED);

    // Set the stack size to something reasonably large. (32K)
    pthread_attr_setstacksize(&attributes, 0x8000);

    // Create a thread.
    pthread_create(&thread, &attributes, (pthread_func_t)runThis, this);

    // Discard temporary attribute object.
    pthread_attr_destroy(&attributes);
}


// Destructor.
PosixThread::~PosixThread()
{
    // Force the thread to terminate if it has not already done so.
    // Is it safe to do this to a thread that has already terminated?
    pthread_cancel(thread);

    // Now wait.
    pthread_join(thread, NULL);
    pthread_detach(&thread);
}


// Main function for threads.
void* PosixThread::runThis(PosixThread* thisThread)
{
    // This code is based on Draft 6 and will have to change.

    // Threads are not cancelled asynchronously.
    pthread_setintrtype(PTHREAD_INTR_CONTROLLED);

    // Enable cancellation.
    pthread_setintr(PTHREAD_INTR_ENABLE);

    // Run the thread.
    thisThread->run();
    return NULL;
}


// Set the thread's priority.
// Return the previous priority.
static int setPriority(pthread_t& thread, int p)
{
    // This code is based on Draft 6 and will have to change.
    pthread_attr_t attributes;
    pthread_getschedattr(thread, &attributes);
    int oldPrio = pthread_attr_getprio(&attributes);
    pthread_attr_setprio(&attributes, p);
    pthread_setschedattr(thread, attributes);

    return oldPrio;
}


// A list of threads.  Used by PosixScheduler.
class ThreadList : private SequentialList
{
public:
    // Add a thread to the list.
    void append(PtThread*);

    // Destructor.  Delete all threads.
    ~ThreadList();
};

 
// Add a thread to the list.
void ThreadList::append(PtThread* t)
{
    SequentialList::append(t);
}


// Destructor.  Delete all threads.
// WARNING! This assumes that the threads have been dynamically allocated.
ThreadList::~ThreadList()
{
    // Delete all the threads in the list.
    for ( int i = size(); i > 0; i--)
    {
	PosixThread* t = (PosixThread*)getAndRemove();
	LOG_DEL; delete t;
    }
}


// System-wide initialization.
PosixScheduler::PosixScheduler()
{
    // Create a new list for registering threads.
    LOG_NEW; threads = new ThreadList;

    // Initialize the thread library.
    pthread_init();

    // Configure the main thread.
    mainThread = pthread_self();

    // Use FIFO scheduling policy (as opposed to round-robin)
    policy = SCHED_FIFO;
    pthread_attr_t attributes;
    pthread_getschedattr(mainThread, &attributes);
    pthread_attr_setsched(&attributes, policy);
    pthread_setschedattr(mainThread, attributes);

    // Record the minimum and maximum possible priorities for the
    // chosen policy.
    minPriority =  sched_get_priority_min(policy);
    maxPriority =  sched_get_priority_max(policy);

    // Set the priority to maximum.
    setPriority(mainThread, maxPriority);
}


PosixScheduler::~PosixScheduler()
{
    LOG_DEL; delete threads;
}


void PosixScheduler::add(PtThread* t)
{
    threads->append(t);
}


// Start or continue the running of all threads.
void PosixScheduler::run()
{
    setPriority(mainThread, minPriority);
    
    // When control returns, restore the priority of this thread to
    // prevent others from running.

    setPriority(mainThread, maxPriority);
}
