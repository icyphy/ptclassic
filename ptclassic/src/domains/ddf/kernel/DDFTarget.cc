/**********************************************************************
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
	s->setParams(numOverlapped,schedulePeriod,maxBufferSize,
		     restructure);
	Target :: setup();
}

DDFTarget::~DDFTarget() { 
	delSched();
}

