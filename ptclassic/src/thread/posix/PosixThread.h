#ifndef _PosixThread_h
#define _PosixThread_h

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
   Created:	23 February 1994
*/

#ifdef __GNUG__
#pragma interface
#endif

#include <pthread.h>
#include "PtThread.h"

class PosixThread : public PtThread
{
public:
    // Create a thread.
    /*virtual*/ void initialize();

    // Terminate the thread.
    /*virtual*/ void terminate();

protected:
    static void* runThis(PosixThread*);

    pthread_t thread;
};


class ThreadList;


class PosixScheduler : public ThreadScheduler
{
public:
    // Constructor.  System-wide initialization.
    PosixScheduler();

    // Destructor.  Terminate all threads.
    /*virtual*/ ~PosixScheduler();

    // Register a thread with this scheduler.
    /*virtual*/ void add(PtThread* t);

    // Allow all registered threads to run (or continue).
    /*virtual*/ void run();

protected:
    ThreadList* threads;
    pthread_t mainThread;

    // Scheduling policy.
    int policy;

    // Priority limits.
    int maxPriority;
    int minPriority;
};

#endif
