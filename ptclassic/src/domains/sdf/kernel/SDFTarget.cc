/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/22/92

 Declaration for SDFTarget, the default target to be used in the SDF
 domain.  This used to be declared elsewhere.

 If loopScheduler is 1, a clustering scheduler is used (SDFClustSched),
 otherwise a default scheduler, SDFScheduler, is used.

***********************************************************************/
static const char file_id[] = "SDFTarget.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFTarget.h"
#include "SDFScheduler.h"
#include "SDFCluster.h"
#include "pt_fstream.h"

SDFTarget::SDFTarget() :
Target("simulate-SDF","SDFStar",
"Runs SDF systems on the local workstation using either the default\n"
"one-processor SDF scheduler or Joe's clustering loop scheduler.")
{
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(loopScheduler.setState("loopScheduler",this,"NO",
			"Specify whether to use loop scheduler."));
	addState(schedulePeriod.setState("schedulePeriod",this,"10000.0",
		"schedulePeriod for interface with a timed domain."));
}

Block* SDFTarget::makeNew() const {
	LOG_NEW; return new SDFTarget;
}

SDFTarget::~SDFTarget() {
	delSched();
}

void SDFTarget::setup() {
	delSched();
	SDFScheduler *s;
	if (int(loopScheduler)) {
		LOG_NEW; s = new SDFClustSched(logFile);
	} else {
		LOG_NEW; s = new SDFScheduler;
	}
	s->schedulePeriod = schedulePeriod;
	s->setGalaxy(*galaxy());
	setSched(s);
	Target::setup();
	if (Scheduler::haltRequested() || logFile.null()) return;

	// create a file with the schedule in it
	pt_ofstream o(logFile);
	if (o) {
		o << scheduler()->displaySchedule() << "\n";
	}
}
