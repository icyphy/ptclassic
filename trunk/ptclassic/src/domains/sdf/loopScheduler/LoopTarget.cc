static const char file_id[] = "LoopTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee, J. Buck, Soonhoi Ha
 Date of creation: 12/8/91

 This target runs simulations using Shuvra's loop scheduler.

*******************************************************************/

#include "Target.h"
#include "KnownTarget.h"
#include "LoopScheduler.h"
#include "StringState.h"
#include "FloatState.h"

class LoopTarget : public Target {
protected:
	StringState logFile;
	FloatState schedulePeriod;
public:
	LoopTarget();
	void start();
	Block* clone() const { LOG_NEW; return &(new LoopTarget)->copyStates(*this);}
	~LoopTarget();
};

LoopTarget::LoopTarget() :
Target("loop-SDF","SDFStar",
"Runs SDF systems on the local workstation using three step looping: \n"
"Decomposition, Joe's clustering, and APEG clustering.")
{
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(schedulePeriod.setState("schedulePeriod",this,"10000.0",
		"schedulePeriod for interface with a timed domain."));
}

LoopTarget::~LoopTarget() {
	delSched();
}

void LoopTarget::start() {
	LOG_NEW; setSched(new LoopScheduler(logFile));
	SDFScheduler* s = (SDFScheduler*) mySched();
	s->schedulePeriod = float(double(schedulePeriod));
}

static LoopTarget loopTargetProto;
static KnownTarget entry(loopTargetProto,"loop-SDF");


