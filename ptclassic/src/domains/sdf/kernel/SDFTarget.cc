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
#include "UserOutput.h"

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

Block* SDFTarget::clone() const {
	LOG_NEW;
	return &(new SDFTarget)->copyStates(*this);
}

SDFTarget::~SDFTarget() {
	delSched();
}

void SDFTarget::start() {
	if (int(loopScheduler)) {
		LOG_NEW; setSched(new SDFClustSched(logFile));
	} else {
		LOG_NEW; setSched (new SDFScheduler);
	}
	SDFScheduler* s = (SDFScheduler*) mySched();
	s->schedulePeriod = float(double(schedulePeriod));
}

int SDFTarget::setup(Galaxy& g) {
	int status = Target::setup(g);
	const char* file = logFile;
	// just return status, unless we want a log file
	// with the default scheduler.
	if (int(loopScheduler) || !status || *file == 0)
		return status;
	UserOutput o;
	if (!o.fileName(logFile)) {
		Error::warn(*this, "can't open log file ", logFile);
	}
	else {
		o << mySched()->displaySchedule();
		o << "\n";
	}
	return status;
}
