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

void DETarget :: start()
{
	if (!mySched()) { LOG_NEW; setSched(new DEScheduler); }

	// scheduler state set-up
	DEScheduler* dSched = (DEScheduler*) mySched();
	dSched->relTimeScale = float(double(timeScale));
	dSched->syncMode = int(syncMode);
}

Block* DETarget :: clone() const  {
	LOG_NEW; 
	return &(new DETarget)->copyStates(*this);
}

DETarget :: ~DETarget() { delSched();}

