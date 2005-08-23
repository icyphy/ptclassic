/**********************************************************************
Version identification:
@(#)FSMTarget.cc	1.8 01/08/99

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:       Bilung Lee
 Date of creation: 3/3/96

 Declaration for FSMTarget.

***********************************************************************/
static const char file_id[] = "FSMTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "InfString.h"
#include "Galaxy.h"
#include "FSMTarget.h"
#include "FSMScheduler.h"

FSMTarget::FSMTarget() :
Target("default-FSM", "FSMStar", "default FSM target")
{
  addState(intlEventNames.setState("intlEventNames", this, "",
	   "Assign the name for each internal event. Each name should be sparated by at least one space."));
  addState(intlEventTypes.setState("intlEventTypes", this, "",
	   "Assign the type for each internal event. Each type should be sparated by at least one space."));

  addState(schedulePeriod.setState("schedulePeriod", this, "0.0",
	   "schedulePeriod for interface with a timed domain."));
}

Block* FSMTarget::makeNew() const {
  LOG_NEW; return new FSMTarget;
}

FSMTarget::~FSMTarget() { delSched(); }

void FSMTarget::setup() {
  int i;
  FSMScheduler* fsmSched = new FSMScheduler;

  // setSched deletes the old scheduler.
  setSched(fsmSched);

  if (intlEventNames.size() != intlEventTypes.size() ) {
      Error::abortRun("FSMTarget: ",
		      "The number of names is not matched",
		      "the number of types for internal events");
  }
  
  fsmSched->intlEventNames.initialize();
  for (i=0; i<intlEventNames.size(); i++)
      fsmSched->intlEventNames << intlEventNames[i];

  fsmSched->intlEventTypes.initialize();
  for (i=0; i<intlEventTypes.size(); i++)
      fsmSched->intlEventTypes << intlEventTypes[i];

//	fsmSched->schedulePeriod = schedulePeriod;

  if (galaxy()) fsmSched->setGalaxy(*galaxy());

  Target::setup();
  if (Scheduler::haltRequested()) return;
}

void FSMTarget::begin() {
  Target::begin();

  ((FSMScheduler *)scheduler())->resetInitState();
}

const char* FSMTarget::domain() {
  return galaxy() ? galaxy()->domain() : "FSM";
}
