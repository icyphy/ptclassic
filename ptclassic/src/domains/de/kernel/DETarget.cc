static const char file_id[] = "DETarget.cc";
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
 Date of creation: 8/18/92

 Declaration for the default DE target. It used to be in DEDomain.cc file.

***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DETarget.h"
#include "DEScheduler.h"

DETarget :: DETarget() : 
Target("default-DE","DEStar","default DE target") 
{
	addState(timeScale.setState("timeScale",this,"1.0",
	    "Relative time scale for interface with another timed domain"));
	addState(syncMode.setState("syncMode",this,"YES",
	"Enforce that the inner timed domain can not be ahead of me in time"));
}

void DETarget :: setup()
{
	if (!scheduler()) { LOG_NEW; setSched(new DEScheduler); }

	// scheduler state set-up
	DEScheduler* dSched = (DEScheduler*) scheduler();
	dSched->setGalaxy(*galaxy());
	dSched->relTimeScale = timeScale;
	dSched->syncMode = syncMode;
	Target :: setup();
}

Block* DETarget :: makeNew() const  {
	LOG_NEW; return new DETarget;
}

DETarget :: ~DETarget() { delSched();}

