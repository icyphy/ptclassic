static const char file_id[] = "CG56ScheduledTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1995 The Regents of the University of California.
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

 Programmer: Asawaree Kalavade

Generate code according to a predefined schedule. A schedule file
is read in and the scheduler is initialized to it.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56ScheduledTarget.h"
#include "KnownTarget.h"
#include "CG56Star.h"
#include "CG56Target.h"
#include "MotorolaTarget.h"
#include "StringState.h"
#include "SDFScheduler.h"
#include <string.h>

// constructor
CG56ScheduledTarget::CG56ScheduledTarget(const char* name,const char* desc) : 
		CG56Target(name,desc), MotorolaTarget(name,desc,"CG56Star") {
	initStates();
}

CG56ScheduledTarget::CG56ScheduledTarget(const CG56ScheduledTarget& arg) : 
		CG56Target(arg), MotorolaTarget(arg) {
	initStates();
	copyStates(arg);
}

void CG56ScheduledTarget :: initStates() {
	addState(schedLoadFile.setState("schedLoadFile",this,"",
		"name of file to load schedule from"));	
}

void CG56ScheduledTarget :: setup() {
	// Generate the schedule file name
	StringList schedFileName;
	char* pathName = expandPathName(destDirectory);
	schedFileName << pathName << "/" << schedLoadFile;	
	delete [] pathName;

	// Open the file name
	FILE* fp = fopen(schedFileName, "r");
	if(fp == NULL) {
		Error::abortRun("Can't read schedule file.");
		return;
	}

	// Read the number of stars
	int numStars = 0;
	if(fscanf(fp, "%d", &numStars) != 1) {
		Error::abortRun("Can't read the number of stars from the schedule.");
		return;
	}

	// Create a new schedule
	SDFSchedule sched;
	for(int i=0; i < numStars; i++) {
		char starName[256];
		if(fscanf(fp, "%s", starName) != 1) {
			Error::abortRun("Can't read the star name from the schedule");
			return;
		}
		starName[255] = 0;
		CG56Star* star = (CG56Star*)starWithFullName(starName);
		if(star == NULL) {
			Error::abortRun("The star '", starName,
					"' was not found in universe");
			return;
		}
		sched.append(*star);
	}
	setSched(new SDFScheduler);
	galaxySetup();

	// schedulerSetup();
	SDFScheduler* sch = (SDFScheduler*)scheduler();
	sch->setGalaxy(*galaxy());
	copySchedule(sched);
	CG56Target :: setup();
}

CG56Star* CG56ScheduledTarget::starWithFullName(const char* name)
{
	Galaxy* gal = galaxy();
	GalStarIter nextStar(*gal);
	CG56Star* star;
	while( (star = (CG56Star*)nextStar++) != 0 )
	{
		if(strcmp(star->fullName(), name) == 0) return star;
	}
	return 0;
}

void CG56ScheduledTarget::headerCode() {
	myCode << "; Target to generate code according to a ";
	myCode << "prespecified schedule\n";
	CG56Target::headerCode();
};

// makeNew
Block* CG56ScheduledTarget :: makeNew () const {
	LOG_NEW; return new CG56ScheduledTarget(*this);
}

/* const char* CG56ScheduledTarget::className() const 
				{ return "CG56ScheduledTarget";} */

ISA_FUNC(CG56ScheduledTarget,CG56Target);
static CG56ScheduledTarget targ("scheduled-CG56",
	"A target for CG56 code generation according to a specified schedule");

static KnownTarget entry(targ,"scheduled-CG56");
