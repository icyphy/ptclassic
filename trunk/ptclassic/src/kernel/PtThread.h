#ifndef _PtThread_h
#define _PtThread_h

/* 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
/* Version $Id$
   Author:	T. M. Parks
   Created:	19 May 1994
*/

#ifdef __GNUG__
#pragma interface
#endif

class PtThread;

class ThreadScheduler
{
public:
    // Register a thread.
    virtual void add(PtThread*) = 0;

    // Allow all threads to run.
    virtual void run() = 0;

    // Delete (and terminate) all threads.
    virtual ~ThreadScheduler() {}
};


class PtThread
{
public:
    // Constructor.
    // Every thread must be registered with a scheduler.
    PtThread(ThreadScheduler& s) { s.add(this); }

    // Destructor.
    // The thread must terminate when it is deleted
    // if it has not done so already.
    virtual ~PtThread() {}

protected:
    // Main function of the thread.
    virtual void run() = 0;
};


#endif
