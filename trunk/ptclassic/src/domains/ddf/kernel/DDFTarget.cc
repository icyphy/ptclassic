/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 4/22/92

 Declaration for DDFTarget, the default target to be used in the DDF
 domain.  This used to be declared elsewhere.

 If restructure is 1, auto-wormholization is performed (default).

***********************************************************************/
static const char file_id[] = "DDFTarget.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFTarget.h"
#include "DDFScheduler.h"

DDFTarget::DDFTarget() :
Target("default-DDF","DataFlowStar","default DDF target")
{
	LOG_NEW; setSched(new DDFScheduler);
	addState(restructure.setState("restructure",this,"YES",
		"perform auto-wormholization?"));
	addState(maxBufferSize.setState("maxBufferSize",this,"1024",
	    "capacity of arc. For the runtime detection of unbounded arc."));
	addState(numOverlapped.setState("numOverlapped",this,"1",
	    "number of iteration cycles to be overlapped for execution."));
	addState(schedulePeriod.setState("schedulePeriod",this,"10000.0",
		"schedulePeriod for interface with a timed domain."));
}

Block* DDFTarget::makeNew() const {
	LOG_NEW; return new DDFTarget;
}

void DDFTarget::setup() {
	// set up the parameters for the DDF scheduler
	DDFScheduler* s = (DDFScheduler*) scheduler();
	s->numOverlapped = int(numOverlapped);
	s->schedulePeriod = float (double(schedulePeriod));
	s->maxToken = int(maxBufferSize);
	int tmp = int(restructure);
	if (!tmp) { s->restructured = TRUE;
		    s->canDom = DDF; }
	else s->restructured = FALSE;
	Target :: setup();
}

DDFTarget::~DDFTarget() { 
	delSched();
}

