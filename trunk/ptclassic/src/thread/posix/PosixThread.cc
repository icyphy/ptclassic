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

// Create a thread.
void PosixThread::initialize()
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


// Terminate the thread
void PosixThread::terminate()
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


// System-wide initialization.
PosixThreadList::PosixThreadList()
{
    // Initialize the thread library.
    // This is done automatically in Solaris and SunOS.
    // pthread_init();

    // Configure the main thread.
    mainThread = pthread_self();
    pthread_attr_init(&mainAttributes);

    // Use FIFO scheduling policy (as opposed to round-robin)
    int policy = SCHED_FIFO;
    pthread_getschedattr(mainThread, &mainAttributes);
    pthread_attr_setsched(&mainAttributes, policy);
    pthread_setschedattr(mainThread, mainAttributes);

    // Record the minimum and maximum possible priorities
    // for the chosen policy.
    minPriority =  sched_get_priority_min(policy);
    maxPriority =  sched_get_priority_max(policy);

    // Set the priority to maximum.
    pthread_getschedattr(mainThread, &mainAttributes);
    pthread_attr_setprio(&mainAttributes, maxPriority);
    pthread_setschedattr(mainThread, mainAttributes);
}

// Start or continue the running of all threads.
void PosixThreadList::run()
{
    // Initialize attributes.
    pthread_attr_init(&mainAttributes);

    // Lower the priority to let other threads run.
    pthread_getschedattr(mainThread, &mainAttributes);
    pthread_attr_setprio(&mainAttributes, minPriority);
    pthread_setschedattr(mainThread, mainAttributes);

    // When control returns, restore the priority of this thread
    // to prevent others from running.
    pthread_getschedattr(mainThread, &mainAttributes);
    pthread_attr_setprio(&mainAttributes, maxPriority);
    pthread_setschedattr(mainThread, mainAttributes);
}
