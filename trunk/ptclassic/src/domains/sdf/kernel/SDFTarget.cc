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
#include "AcyLoopScheduler.h"

SDFTarget::SDFTarget(const char* nam, const char* desc,
		     const char* assocDomain) :
Target(nam,"SDFStar",desc,assocDomain)
{
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(loopScheduler.setState("loopScheduler",this,"1",
			"Specify whether to use loop scheduler."));
	addState(schedulePeriod.setState("schedulePeriod",this,"0.0",
			"schedulePeriod for interface with a timed domain."));
}

Block* SDFTarget::makeNew() const {
	LOG_NEW; return new SDFTarget(name(), descriptor(),
				      getAssociatedDomain());
}

SDFTarget::~SDFTarget() {
	delSched();
}

void SDFTarget::setup() {
	delSched();
	SDFScheduler *s;
	if (int(loopScheduler) == 3) {
	    // Determine if the graph is acyclic.  It not, use
	    // another loop scheduler.
	    // FIXME:
	    // NOTE: Error::message will halt the simulation
	    // till the message is dismissed.  This will be fixed
	    // later on when we use something to post messages that
	    // does not require dismissal.
	    if (galaxy()) {
	    	if (!isAcyclic(galaxy(), 0)) {
		    StringList message;
		    message << "The scheduler option that you selected, 3, "
		    	    << "in the SDFTarget parameters represents a "
		    	    << "loop scheduler that only works on acyclic "
		    	    << "graphs.  Since this graph is not acyclic "
		    	    << "another scheduler will be used (corresponding"
		    	    << " to option 1.\n";
		    Error::message(message);
		    loopScheduler.setCurrentValue("1");
		}
	    }
	}
	switch(int(loopScheduler)) {
	    case 0:
		LOG_NEW; s = new SDFScheduler;
		break;
	    case 1:
		LOG_NEW; s = new SDFClustSched(logFile);
		break;
	    case 2:
		LOG_NEW; s = new SDFClustSched(logFile);
		break;
	    case 3:
		LOG_NEW; s = new AcyLoopScheduler(logFile);
		break;
	    default:
		Error::abortRun(*this, "Unknown scheduler");
		return;
	}
	s->schedulePeriod = schedulePeriod;
	if (galaxy()) s->setGalaxy(*galaxy());
	setSched(s);
	Target::setup();
	if (Scheduler::haltRequested() || logFile.null()) return;

	// create a file with the schedule in it
	pt_ofstream o(logFile);
	if (o) {
		o << scheduler()->displaySchedule() << "\n";
	}
}
