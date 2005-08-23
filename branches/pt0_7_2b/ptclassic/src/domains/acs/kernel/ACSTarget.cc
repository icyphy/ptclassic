static const char file_id[] = "ACSTarget.cc";
/**********************************************************************
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED
MARTIN COMPANY, BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
OR SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA AND SANDERS, A LOCKHEED MARTIN COMPANY
SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED MARTIN COMPANY
HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.  COPYRIGHTENDKEY


 Programmers:  Eric Pauer (Sanders), Christopher Hylands, Edward A. Lee
 Date of creation: 1/15/98
 Version: @(#)ACSTarget.cc	1.11 09/10/99

***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include <stdio.h>              // printf()

#include "ACSTarget.h"
#include "KnownTarget.h"
#include "pt_fstream.h"
#include "Tokenizer.h"
#include "AcyLoopScheduler.h"


ACSTarget::ACSTarget(const char* name,const char* starclass, const char* desc,
        const char* category, const char* assocDomain) :
    HLLTarget(name,starclass,desc,assocDomain) {

      if (DEBUG_ACSTARGET)
	printf("ACSTarget::ACSTarget invoked\n");

	// This determines the core used in simulation.
	addState(coreCategory.setState("Core Category", this, category, "Sets the core category."));
	coreCategory.clearAttributes(A_SETTABLE);

	// These are from the SDF simulation Target.
	addState(logFile.setState("logFile",this,"",
			"Log file to write to (none if empty)"));
	addState(schedulePeriod.setState("schedulePeriod",this,"0.0",
			"schedulePeriod for interface with a timed domain."));

    }

ACSTarget :: ~ACSTarget() 
{
  if (DEBUG_ACSTARGET)
    printf("ACSTarget::~ACSTarget invoked\n");
}

// Copied directly from the SDF default target.
void ACSTarget::setup() {
	delSched();
	SDFScheduler *s;
	pt_ofstream o;

	if (DEBUG_ACSTARGET)
	  printf("ACSTarget::setup() invoked\n");

	// create a file with the schedule in it
	if (!logFile.null()) o.open(logFile);

	const char* tmpname = loopingLevel;
	Tokenizer lexer(tmpname,"#");
	char buf[1024];
	lexer >> buf;
	loopingLevel.setCurrentValue(buf);

	if (strcasecmp(buf,"ACYLOOP") == 0) {
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
		    message << "The scheduler option that you selected, "
			    << "ACYLOOP, "
		    	    << "in the SDFTarget parameters represents a "
		    	    << "loop scheduler that only works on acyclic "
		    	    << "graphs.  Since this graph is not acyclic "
		    	    << "the scheduler CLUST will be used "
			    << "(corresponding to old option 1).\n";
		    if (o) o << message;
		    loopingLevel.setCurrentValue("CLUST");
		}
	    }
	}
	const char* sname = loopingLevel;
	if (strcasecmp(sname,"DEF")==0 || strcmp(sname,"0")==0 || 
		strcasecmp(sname,"NO") == 0) {
	    s = new SDFScheduler;
	} else if (strcasecmp(sname,"CLUST")==0 || strcmp(sname,"1")==0 || 
		strcasecmp(sname,"YES")==0 ) {
	    s = new SDFClustSched(logFile);
	} else if (strcasecmp(sname,"ACYLOOP") == 0) {
	    s = new AcyLoopScheduler(logFile);
	} else {
	    Error::abortRun(*this, "Unknown scheduler.");
	    return;
	}
	s->schedulePeriod = schedulePeriod;
	if (galaxy()) s->setGalaxy(*galaxy());
	setSched(s);
	Target::setup();
	if (Scheduler::haltRequested() || logFile.null()) return;

	if (o) {
		o << scheduler()->displaySchedule() << "\n";
	}
}


void ACSTarget::wrapup() { if ( isCG() ) HLLTarget::wrapup(); else Target::wrapup(); }  // FIXME: Code generation define something different here.

Block* ACSTarget::makeNew() const {
    LOG_NEW; return new ACSTarget(name(),starType(),descriptor());
}

ISA_FUNC(ACSTarget,HLLTarget);

// Maps Plasma type from Float to Fix for Fixed-Point simulation.
DataType ACSTarget::mapType(DataType type) {
	if ( isFixedPoint() && (strcmp(type,FLOAT) == 0) ) {
		return FIX;
	}
	return type;
}

