#ifndef _PosixCondition_h
#define _PosixCondition_h

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
#include "PtCondition.h"

class PosixMonitor;

class PosixCondition : public PtCondition
{
public:
    // Constructor.
    PosixCondition(PosixMonitor&);

    // Destructor.
    /*virtual*/ ~PosixCondition();

    // Wait for notification.
    /*virtual*/ void wait();

    // Give notification to a single waiting thread.
    /*virtual*/ void notify();

    // Give notification to all waiting threads.
    /*virtual*/ void notifyAll();

protected:
    pthread_cond_t condition;
    pthread_mutex_t& mutex;
};

#endif
