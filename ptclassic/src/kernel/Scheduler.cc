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

#include "Scheduler.h"

void Scheduler::resetStopTime(float limit) {
	setStopTime(limit);
}

void Scheduler::setCurrentTime(float val) {
	currentTime = val;
}

StringList Scheduler::displaySchedule() {
	return "displaySchedule not implemented for this scheduler\n";
}

const char* Scheduler::domain() const {
	return "Undefined";
}
