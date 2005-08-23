/**********************************************************************
Version identification:
@(#)DETarget.h	1.14    04/19/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha
 Date of creation: 8/18/92

 Declaration for the default DE target. It used to be in DEDomain.cc file.

 Modified: John Davis
 Date: 12/16/97
        Facilities have been added to DETarget to allow for
        mutability. In particular, a parameter is set which
        allows the MutableCQScheduler to be used instead
        of the DEScheduler or CQScheduler. Key methods and
        members are:

                IntState mutableQ;

***********************************************************************/
#ifndef _DETarget_h
#define _DETarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "IntState.h"
#include "FloatState.h"
#include "DEScheduler.h"

class DETarget : public Target {
public:
	// constructor
	DETarget();

	// destructor
	~DETarget();

	// return a new copy
	/* virtual */ Block* makeNew() const;

	// call the begin method on the galaxy
	/* virtual */ void begin();

	// Return TRUE if the MutableCalendarQueue is being used
	int isMutable();

        // Let outside domain know whether to refire.
	/*virtual*/ int selfFiringRequested() {
            // Invoke scheduler to decide.
            DEBaseSched* sched = (DEBaseSched*)scheduler();
            return sched->selfFiringRequested();
        }
	
	// If selfFiringRequested returns TRUE, return the time at which
	// this firing is requested.
	/*virtual*/ double nextFiringTime () {
            // Invoke scheduler to decide.
            DEBaseSched* sched = (DEBaseSched*)scheduler();
            return sched->nextFiringTime();
        }

	// Return TRUE if the ResourceContentionQueue is being used
	int isResource();

protected:
	void setup();

	// relative time scale for interface with another timed domain.
	FloatState timeScale;

	// If TRUE, inner timed domain cannot be ahead of outer DE domain
	IntState syncMode;

	// Use CalendarQueue scheduler if TRUE;
	IntState calQ;

        // Use MutableCalendarQueue scheduler if TRUE;
        IntState mutableQ;

        //Use DERCScheduler for Resource Contention, if TRUE;
        IntState dercQ;

};

#endif
