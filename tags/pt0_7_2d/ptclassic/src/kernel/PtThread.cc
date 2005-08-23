static const char file_id[] = "PtThread.cc";

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
/* Version @(#)PtThread.cc	1.1 3/4/96
   Author:	T. M. Parks
   Created:	29 Feb 1996
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "PtThread.h"

// Add a thread to the list.
void ThreadList::add(PtThread* t)
{
    SequentialList::append(t);
}

// Destructor.  Delete (and terminate) all threads.
// WARNING! This assumes that the threads have been dynamically allocated.
ThreadList::~ThreadList()
{
    for (int i = size(); i > 0; i--)
    {
	PtThread* t = (PtThread*)getAndRemove();
	t->terminate();
	LOG_DEL; delete t;
    }
}

ThreadListIter::ThreadListIter(ThreadList& l) : ListIter(l) {}
