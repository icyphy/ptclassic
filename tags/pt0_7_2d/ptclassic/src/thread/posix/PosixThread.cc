static const char file_id[] = "PosixThread.cc";

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
/* Version @(#)PosixThread.cc	1.11 03/26/98
   Author:	T. M. Parks
   Created:	21 October 1994
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "PosixThread.h"
#include "compat.h"

#ifdef PTHPUX10
#define pthread_attr_init pthread_attr_create
#define pthread_attr_destroy pthread_attr_delete
const int minPriority = PRI_FIFO_MIN;
const int maxPriority = PRI_FIFO_MAX;
#else
const int minPriority =  sched_get_priority_min(SCHED_FIFO);
const int maxPriority =  sched_get_priority_max(SCHED_FIFO);
#endif

// Initialize the main thread.
static pthread_t initMainThread()
{
#if !defined(PTHPUX10) && !defined(_PTHREAD_1003_1c)
    // Be sure that thread library has already been initialized.
    pthread_init();
#endif
    // Get the id of the main thread.
    pthread_t thread = pthread_self();

    // Use FIFO scheduling policy (as opposed to round-robin)
    // Set the priority to maximum.
#ifdef PTHPUX10
    pthread_setscheduler(thread, SCHED_FIFO, maxPriority);
#else // PTHPUX10
#ifdef _PTHREAD_1003_1c
    struct sched_param param;
    int policy;
    pthread_getschedparam(thread, &policy, &param);
    policy = SCHED_FIFO;
    param.sched_priority = maxPriority;
    pthread_setschedparam(thread, policy, &param);
#else // _PTHREAD_1003_1c
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_getschedattr(thread, &attributes);
    pthread_attr_setsched(&attributes, SCHED_FIFO);
    pthread_attr_setprio(&attributes, maxPriority);
    pthread_setschedattr(thread, attributes);
    pthread_attr_destroy(&attributes);
#endif // _PTHREAD_1003_1c
#endif // PTHPUX10
    return thread;
}

const pthread_t mainThread = initMainThread();

// Create a thread.
void PosixThread::initialize()
{
    // Initialize attributes.
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
#ifdef _PTHREAD_1003_1c
    pthread_attr_setschedpolicy(&attributes, SCHED_FIFO);
    struct sched_param param;
    param.sched_priority = maxPriority;
    pthread_attr_setschedparam(&attributes, &param);    
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
    pthread_attr_setstacksize(&attributes, 0x8000);
#endif
#else // _PTHREAD_1003_1c
    pthread_attr_setsched(&attributes, SCHED_FIFO);
    pthread_attr_setprio(&attributes, maxPriority);
    pthread_attr_setstacksize(&attributes, 0x8000);
#endif // _PTHREAD_1003_1c
    // Create a thread.
#ifdef PTHPUX10
    pthread_create(&thread, attributes, (pthread_startroutine_t)runThis, this);
#else
#ifdef _PTHREAD_1003_1c
    pthread_create(&thread, &attributes, runThis, this);
#else
    pthread_create(&thread, &attributes, (pthread_func_t)runThis, this);
#endif // _PTHREAD_1003_1c
#endif

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
#ifdef _PTHREAD_1003_1c
    pthread_detach(thread);
#else
    pthread_detach(&thread);
#endif
}


// Run a thread.
void* PosixThread::runThis(PosixThread* thisThread)
{
    thisThread->run();
    return NULL;
}


// Start or continue the running of all threads.
void PosixThread::runAll()
{
    // Lower the priority to let other threads run.  When control
    // returns, restore the priority of this thread to prevent others
    // from running.

#ifdef PTHPUX10
    pthread_setprio(mainThread, minPriority);

    pthread_setprio(mainThread, maxPriority);
#else
#ifdef _PTHREAD_1003_1c
    struct sched_param param;
    int policy;
    pthread_getschedparam(mainThread, &policy, &param);

    param.sched_priority = minPriority;
    pthread_setschedparam(mainThread, policy, &param);

    param.sched_priority = maxPriority;
    pthread_setschedparam(mainThread, policy, &param);    
#else // _PTHREAD_1003_1c
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_getschedattr(mainThread, &attributes);

    pthread_attr_setprio(&attributes, minPriority);
    pthread_setschedattr(mainThread, attributes);

    pthread_attr_setprio(&attributes, maxPriority);
    pthread_setschedattr(mainThread, attributes);

    pthread_attr_destroy(&attributes);
#endif // _PTHREAD_1003_1c
#endif
}
