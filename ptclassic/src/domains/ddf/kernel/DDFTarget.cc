static const char file_id[] = "DDFTarget.cc";
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


 Programmer: Soonhoi Ha
 The authors gratefully acknowledge the contributions of Richard Stevens.


 Declaration for DDFTarget, the default target to be used in the DDF
 domain.

 If useOldScheduler is non-zero, then the DDF scheduler from the 0.5.2 release
 is used.  Otherwise, if restructure is non-zero, SDF stars are clustered
 and scheduled ahead of time.  Otherwise, the DDFSimpleSched is used.
 The latter is the default.

***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DDFTarget.h"
#include "DDFScheduler.h"
#include "DDFClustSched.h"
#include "DDFSimpleSched.h"
#include "InfString.h"

DDFTarget::DDFTarget() :
Target("default-DDF","DataFlowStar","default DDF target")
{
	addState(maxBufferSize.setState("maxBufferSize",this,"1024",
	    "capacity of an arc. For the runtime detection of unbounded arc."));
	addState(schedulePeriod.setState("schedulePeriod",this,"0.0",
	    "Time of one iteration, for interface with a timed domains."));
	addState(runUntilDeadlock.setState("runUntilDeadlock",this,"NO",
	    "In each iteration, run until the system deadlocks."));
	addState(restructure.setState("restructure",this,"NO",
	    "Cluster SDF actors and pre-schedule (experimental!)"));
	addState(useOldScheduler.setState("useOldScheduler",this,"NO",
	    "Use the DDF scheduler from the 0.5.2 release of Ptolemy."));
	addState(numOverlapped.setState("numOverlapped",this,"1",
	    "Number of iteration cycles to be overlapped (used by old scheduler only)"));
	addState(logFile.setState("logFile",this,"",
            "Log file to write to (none if empty). Used only by the restructuring scheduler."));
	LOG_NEW; firings = new TextTable;
}

Block* DDFTarget::makeNew() const {
	LOG_NEW; return new DDFTarget;
}

void DDFTarget::setup() {
	DDFScheduler* s;
	if (int(useOldScheduler)) {
	  LOG_NEW; s = new DDFScheduler;
	} else {
	  if (int(restructure)) {
	    LOG_NEW; DDFClustSched* cs = new DDFClustSched(logFile);
	    cs->deadlockIteration(int(runUntilDeadlock));
	    s = cs;
	  } else {
	    LOG_NEW; DDFSimpleSched* ss = new DDFSimpleSched();
	    ss->deadlockIteration(int(runUntilDeadlock));
	    s = ss;
	  }
	}
	setSched(s);

	// set up the parameters for the DDF scheduler
	s->setParams(numOverlapped,schedulePeriod,maxBufferSize);
	Target :: setup();
}

DDFTarget::~DDFTarget() { 
	delSched();
	LOG_DEL; delete firings;
}

// Redefine Block::clone method to copy the firings table.
Block* DDFTarget::clone() const {
    // Create a new DDFTarget object using the Block clone method
    // (which in turn uses makeNew(), defined above).
    DDFTarget* t = (DDFTarget*)Block::clone();

    // Copy the contents of the firings table.
    t->setFirings(*firings);

    return (Block*) t;
}

void DDFTarget::setFirings(TextTable& tbl) {
    LOG_DEL; delete firings;
    LOG_NEW; firings = new TextTable(tbl);
}

StringList DDFTarget::pragma (const char* parentname,
			      const char* blockname) const {
    InfString compoundname;
    compoundname << parentname << "." << blockname;
    const char* value = firings->lookup(compoundname);
    if (value) {
	StringList ret = "firingsPerIteration ";
	ret += value;
	return ret;
    } else {
	return "";
    }
}

StringList DDFTarget::pragma (const char* parentname,
			      const char* blockname,
			      const char* pragmaname) const {
  const char* c;
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  if (strcmp(pragmaname,"firingsPerIteration") != 0 ||
      !(c = firings->lookup(compoundname))) {
    c = "";
  }
  return c;
}

StringList DDFTarget::pragma (const char* parentname,
			      const char* blockname,
			      const char* pragmaname,
			      const char* value) {
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  if (strcmp(pragmaname,"firingsPerIteration") == 0) {
    firings->insert(compoundname, value);
  }
  return "";
}

const char* DDFTarget::className() const { return "DDFTarget";}
ISA_FUNC(DDFTarget,Target);
