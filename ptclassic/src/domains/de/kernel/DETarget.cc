static const char file_id[] = "DETarget.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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

