static const char file_id[] = "LoopTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

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
	void setup();
	Block* makeNew() const { LOG_NEW; return new LoopTarget;}
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

void LoopTarget::setup() {
	if (!scheduler()) {
		LOG_NEW; setSched(new LoopScheduler(logFile));
	}
	SDFScheduler* s = (SDFScheduler*) scheduler();
	s->schedulePeriod = float(double(schedulePeriod));
	s->setGalaxy(*galaxy());
	Target::setup();
}

static LoopTarget loopTargetProto;
static KnownTarget entry(loopTargetProto,"loop-SDF");


