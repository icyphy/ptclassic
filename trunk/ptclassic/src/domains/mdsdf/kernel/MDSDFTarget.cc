/**********************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  Mike J. Chen
 Date of creation: 9/1/93

 Declaration for MDSDFTarget, the default target to be used in the MDSDF
 domain.

***********************************************************************/
static const char file_id[] = "MDSDFTarget.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFTarget.h"
#include "MDSDFScheduler.h"
#include "pt_fstream.h"

MDSDFTarget::MDSDFTarget() :
Target("simulate-MDSDF","MDSDFStar",
"Runs MDSDF systems on the local workstation using the default scheduler.")
{
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(schedulePeriod.setState("schedulePeriod",this,"10000.0",
		"schedulePeriod for interface with a timed domain."));
}

Block* MDSDFTarget::makeNew() const {
	LOG_NEW; return new MDSDFTarget;
}

MDSDFTarget::~MDSDFTarget() {
	delSched();
}

void MDSDFTarget::setup() {
	delSched();
	MDSDFScheduler *s;
        LOG_NEW; s = new MDSDFScheduler;
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
