static const char file_id[] = "DETarget.cc";
/**********************************************************************
Version identification:
@(#)DETarget.cc	1.21 04/19/99

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
#ifdef __GNUG__
#pragma implementation
#endif

#include "DETarget.h"
#include "DEScheduler.h"
#include "CQScheduler.h"
#include "MutableCQScheduler.h"
#include "DERCScheduler.h"
#include "GalIter.h"

// Defined in DEDomain.cc
extern const char DEdomainName[];

DETarget :: DETarget() : 
Target("default-DE", "DEStar", "default DE target", DEdomainName) {
	addState(timeScale.setState("timeScale",this,"1.0",
	    "Relative time scale for interface with another timed domain"));

	addState(syncMode.setState("syncMode",this,"YES",
	"Enforce that the inner timed domain cannot be ahead of me in time"));

	addState(calQ.setState("calendar queue scheduler?", this, "YES",
	    "Use the CalendarQueue scheduler."));

        addState(mutableQ.setState("mutable calendar queue scheduler?", this,
                "NO", 
                "Setting the Mutable CalendarQueue will override the regular CalendarQueue setting."));

        addState(dercQ.setState("Resource Contention scheduler?", this,
                "NO", 
                "Setting the Resource Contention scheduler will override the regular CalendarQueue settings."));
}

void DETarget :: setup() {
	DEBaseSched* dSched;

        if(int(dercQ)) {
            LOG_NEW; dSched = new DERCScheduler;
        }
        else if (int(mutableQ)) {
                LOG_NEW; dSched = new MutableCQScheduler;
        }
        else if (int(calQ)) {
                LOG_NEW; dSched = new CQScheduler;
        }
        else {
                LOG_NEW; dSched = new DEScheduler;
        }

	// setSched deletes the old scheduler.
	setSched(dSched);
	if (galaxy()) dSched->setGalaxy(*galaxy());
	dSched->relTimeScale = timeScale;
	dSched->syncMode = int(syncMode);
	Target :: setup();
}

// Invoke begin methods.  Some DE stars, such those derived from
// DERepeatStar, generate output events in their begin methods.
// Make sure these events get into the event queue.
void DETarget::begin() {
	Galaxy *gal = galaxy();
	if (!gal) return;
	GalStarIter nextStar(*gal);
	Star *s;
	while ((s = nextStar++) != 0) {
		s->begin();
		if (SimControl::haltRequested()) return;
		// call sendOutput here to get events into the event queue
		((DEStar*)s)->sendOutput();
	}
}

int DETarget::isMutable() {
	return int(mutableQ);
}

int DETarget::isResource() {
	return int(dercQ);
}

Block* DETarget :: makeNew() const  {
	LOG_NEW; return new DETarget;
}

DETarget :: ~DETarget() { delSched();}









