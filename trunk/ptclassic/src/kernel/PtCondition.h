#ifndef _PtCondition_h
#define _PtCondition_h

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
   Created:	18 June 1994

   PtCondition is a condition variable for use with monitors.
*/
#ifdef __GNUG__
#pragma interface
#endif

class PtGate;

class PtCondition
{
public:
    // Constructor.
    PtCondition(PtGate& g) : mon(g) {}

    // Destructor.
    // Any threads still waiting on this condition are deleted.
    virtual ~PtCondition() {}

    // Wait for notification.
    virtual void wait() = 0;

    // Give notification to a single waiting thread.
    virtual void notify() = 0;

    // Give notification to all waiting threads.
    virtual void notifyAll() = 0;

    // Monitor that this condition variable is associated with.
    PtGate& monitor() { return mon; }

protected:
    PtGate& mon;
};

#endif
