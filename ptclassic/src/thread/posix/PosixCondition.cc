static const char file_id[] = "PosixCondition.cc";

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
/* Version @(#)PosixCondition.cc	1.3 3/7/96
   Author:	T. M. Parks
   Created:	21 October 1994
*/


#ifdef __GNUG__
#pragma implementation
#endif

#include "PosixCondition.h"
#include "PosixMonitor.h"
#include "type.h"

// Constructor.
PosixCondition::PosixCondition(PosixMonitor& m)
    : PtCondition(m), mutex(m.mutex)
{
#ifdef PTHPUX10
    pthread_cond_init(&condition, pthread_condattr_default);
#else
    pthread_cond_init(&condition, NULL);
#endif
}

// Destructor.
/*virtual*/ PosixCondition::~PosixCondition()
{
    pthread_cond_destroy(&condition);
}

// Wait for notification.
/*virtual*/ void PosixCondition::wait()
{
    // Guarantee that the mutex will not remain locked by a cancelled thread.
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, &mutex);

    pthread_cond_wait(&condition, &mutex);

    // Remove cleanup handler, but do not execute.
    pthread_cleanup_pop(FALSE);
}

// Give notification to a single waiting thread.
/*virtual*/ void PosixCondition::notify()
{
    pthread_cond_signal(&condition);
}

// Give notification to all waiting threads.
/*virtual*/ void PosixCondition::notifyAll()
{
    pthread_cond_broadcast(&condition);
}
