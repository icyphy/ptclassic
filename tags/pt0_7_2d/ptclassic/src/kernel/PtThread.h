#ifndef _PtThread_h
#define _PtThread_h

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
/* Version @(#)PtThread.h	1.8 3/7/96
   Author:	T. M. Parks
   Created:	19 May 1994
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"

class PtThread;

// A list of threads.
class ThreadList : private SequentialList
{
    friend class ThreadListIter;

public:
    // Register a thread.
    void add(PtThread*);

    // Delete (and terminate) all threads.
    virtual ~ThreadList();
};


// An iterator for ThreadList.
class ThreadListIter : private ListIter
{
public:
    ThreadListIter(ThreadList&);
    inline PtThread* next() { return (PtThread*)ListIter::next();}
    inline PtThread* operator++(POSTFIX_OP) { return next();}
    ListIter::reset;
};


// Abstract base class for threads.
class PtThread
{
public:
    // Create a thread.
    virtual void initialize() = 0;

    // Terminate the thread.
    virtual void terminate() = 0;

    // Run all threads.  Do nothing in base class.
    static void runAll() {};

protected:
    // Main function of the thread.
    virtual void run() = 0;
};


#endif
