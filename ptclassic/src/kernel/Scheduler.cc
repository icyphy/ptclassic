static const char file_id[] = "Scheduler.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck, S. Ha

 Methods for the Scheduler baseclass

**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Target.h"
#include "Scheduler.h"
#include "Error.h"
#include "Galaxy.h"

void Scheduler :: resetStopTime(double limit) {
	setStopTime(limit);
}

void Scheduler :: setCurrentTime(double val) {
	currentTime = val;
}

StringList Scheduler :: displaySchedule() {
	return "displaySchedule not implemented for this scheduler\n";
}

// Used if not overriden
const char* Scheduler :: domain() const {
        return "Undefined";
}

void Scheduler :: setTarget(Target& t) {
	myTarget = &t;
}

Target& Scheduler :: target() {
	return *myTarget;
}

void Scheduler :: setGalaxy(Galaxy& g) { myGalaxy = &g;}

// Return a StringList with code that can be executed to
// effect a run.  In the base class, this just causes an error.
void Scheduler :: compileRun() {
	Error :: abortRun ("This scheduler doesn't know how to compile.");
}

