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
	addState(allowDynamic.setState("allowDynamic",this,"NO",
		"Specify whether to try dynamic execution if the graph\n"
					"cannot be completely clustered"));
	addState(requireStronglyConsistent.setState(
		"requireStronglyConsistent",this,"NO",
		"Enable 'strongly consistent' check: will reject some valid systems"));
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
	LOG_NEW; BDFClustSched* s = new BDFClustSched(logFile,allowDynamic,
						      requireStronglyConsistent);
	setSched(s);
	s->schedulePeriod = schedulePeriod;
	Target::setup();
}
