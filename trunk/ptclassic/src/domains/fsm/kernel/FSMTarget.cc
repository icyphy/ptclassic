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

#include "InfString.h"
#include "Galaxy.h"
#include "FSMTarget.h"
#include "FSMScheduler.h"

FSMTarget::FSMTarget() :
Target("default-FSM", "FSMStar", "default FSM target")
{
  addState(inputNameMap.setState("inputNameMap", this, "",
	   "Assign the name for each PortHole in the input MultiPortHole. Each name should be embraced in a pair of double quotes."));
  addState(outputNameMap.setState("outputNameMap", this, "",
	   "Assign the name for each PortHole in the output MultiPortHole. Each name should be embraced in a pair of double quotes."));
  addState(internalNameMap.setState("internalNameMap", this, "",
	   "Assign the name for each internal event. Each name should be embraced in a pair of double quotes."));
  addState(machineType.setState("machineType", this, "Pure",
	   "Choices: Pure, Valued."));
  addState(evaluationType.setState("evaluationType", this, "Strict",
	   "Specify how this FSM will be evaluated: Strict or NonStrict."));
  addState(oneWriterType.setState("oneWriterType", this, "Compile",
	   "Specify when to do the one-writer rule checking."));
  addState(schedulePeriod.setState("schedulePeriod", this, "0.0",
	   "schedulePeriod for interface with a timed domain."));
}

Block* FSMTarget::makeNew() const {
  LOG_NEW; return new FSMTarget;
}

FSMTarget::~FSMTarget() { delSched(); }

void FSMTarget::setup() {
  FSMScheduler* fsmSched;
  InfString buf = (const char*)evaluationType;
  buf << (const char*)machineType;
  if (!strcmp(buf,"StrictPure")) {
    fsmSched = new SPureSched;
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
  delete [] fsmSched->evaluationType;
  fsmSched->evaluationType = savestring(evaluationType);
  delete [] fsmSched->oneWriterType;
  fsmSched->oneWriterType = savestring(oneWriterType);
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
