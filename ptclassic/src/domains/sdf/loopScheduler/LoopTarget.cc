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
#include "IntState.h"

class LoopTarget : public Target {
protected:
	StringState logFile;
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
}

LoopTarget::~LoopTarget() {
	delSched();
}

void LoopTarget::start() {
	LOG_NEW; setSched(new LoopScheduler(logFile));
}

static LoopTarget loopTargetProto;
static KnownTarget entry(loopTargetProto,"loop-SDF");


