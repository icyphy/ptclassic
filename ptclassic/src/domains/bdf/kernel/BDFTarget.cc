/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck

 Declaration for BDFTarget, the default target to be used in the BDF
 domain.  This used to be declared elsewhere.

 If loopScheduler is 1, a clustering scheduler is used (BDFClustSched),
 otherwise a default scheduler, BDFScheduler, is used.

***********************************************************************/
static const char file_id[] = "BDFTarget.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "BDFTarget.h"
#include "BDFScheduler.h"
#include "BDFCluster.h"
#include "pt_fstream.h"

BDFTarget::BDFTarget() :
Target("simulate-BDF","DataFlowStar",
"Runs BDF systems on the local workstation using either the default\n"
"one-processor BDF scheduler or Joe's clustering loop scheduler.")
{
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(loopScheduler.setState("loopScheduler",this,"YES",
			"Specify whether to use loop scheduler."));
	addState(schedulePeriod.setState("schedulePeriod",this,"10000.0",
		"schedulePeriod for interface with a timed domain."));
}

Block* BDFTarget::makeNew() const {
	LOG_NEW; return new BDFTarget;
}

BDFTarget::~BDFTarget() {
	delSched();
}

void BDFTarget::setup() {
	SDFScheduler* s;
	if (int(loopScheduler)) {
		LOG_NEW; s = new BDFClustSched(logFile);
	} else {
		LOG_NEW; s = new BDFScheduler;
	}
	setSched(s);
	s->schedulePeriod = schedulePeriod;
	Target::setup();
	const char* file = logFile;
	// just return status, unless we want a log file
	// with the default scheduler.
	if (int(loopScheduler) || Scheduler::haltRequested() || *file == 0)
		return;
	// create a file with the schedule in it
	pt_ofstream o(logFile);
	if (o) o << scheduler()->displaySchedule() << "\n";
}
