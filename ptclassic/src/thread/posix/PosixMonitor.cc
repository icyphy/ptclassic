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

#include "PosixMonitor.h"
#include "logNew.h"

// Constructor.
PosixMonitor::PosixMonitor()
{
    pthread_mutex_init(&mutex, NULL);
}

// Destructor.
/*virtual*/ PosixMonitor::~PosixMonitor()
{
    pthread_mutex_destroy(&mutex);
}

// Make a new PtGate of this type.
/*virtual*/ PtGate* PosixMonitor::makeNew() const
{
    LOG_NEW; return new PosixMonitor;
}

// Obtain exclusive use of the lock.
/*virtual*/ void PosixMonitor::lock()
{
    pthread_mutex_lock(&mutex);

    // Guarantee that the mutex will not remain locked by a cancelled thread.
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, &mutex);
}

// Release the lock.
/*virtual*/ void PosixMonitor::unlock()
{
    // Remove cleanup handler and unlock.
    pthread_cleanup_pop(TRUE);
}
