/**********************************************************************
Version identification:
$Id$

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

 Programmer:       Bilung Lee
 Date of creation: 3/3/96

 Declaration for FSMTarget.

***********************************************************************/
static const char file_id[] = "FSMTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Galaxy.h"
#include "FSMTarget.h"
#include "FSMScheduler.h"
#include "MixedScheduler.h"

FSMTarget::FSMTarget() :
Target("default-FSM", "FSMStar", "default FSM target")
{
  addState(inputNameMap.setState("inputNameMap", this, "",
	   "Assign the name for each PortHole in the input MultiPortHole. Each name should be embraced in a pair of double quotes."));
  addState(outputNameMap.setState("outputNameMap", this, "",
	   "Assign the name for each PortHole in the output MultiPortHole. Each name should be embraced in a pair of double quotes."));
  addState(internalNameMap.setState("internalNameMap", this, "",
	   "Assign the name for each internal event. Each name should be embraced in a pair of double quotes."));
  addState(machineType.setState("machineType", this, "Moore",
	   "Moore or Mealy machine."));
  addState(schedulePeriod.setState("schedulePeriod", this, "0.0",
	   "schedulePeriod for interface with a timed domain."));
}

Block* FSMTarget::makeNew() const {
  LOG_NEW; return new FSMTarget;
}

FSMTarget::~FSMTarget() { delSched(); }

void FSMTarget::setup() {
  FSMScheduler* fsmSched;
  if (!strcmp(machineType,"Mixed")) {
    fsmSched = new MixedScheduler;
  } else if (!strcmp(machineType,"Moore") || !strcmp(machineType,"Mealy")) {
    fsmSched = new BasicScheduler;
  } else {
      Error::abortRun("FSMTarget: ", 
		      "Unregconized machine type!");
      return; 
  }

  // setSched deletes the old scheduler.
  setSched(fsmSched);

  delete [] fsmSched->inputNameMap;
  fsmSched->inputNameMap = savestring(inputNameMap);
  delete [] fsmSched->outputNameMap;
  fsmSched->outputNameMap = savestring(outputNameMap);
  delete [] fsmSched->internalNameMap;
  fsmSched->internalNameMap = savestring(internalNameMap);
  delete [] fsmSched->machineType;
  fsmSched->machineType = savestring(machineType);
//	fsmSched->schedulePeriod = schedulePeriod;

  if (galaxy()) fsmSched->setGalaxy(*galaxy());

  Target::setup();
  if (Scheduler::haltRequested()) return;
}

const char* FSMTarget::domain() {
  return galaxy() ? galaxy()->domain() : "FSM";
}
