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

#include <signal.h>
#include "Scheduler.h"
#include "Error.h"

void Scheduler::resetStopTime(float limit) {
	setStopTime(limit);
}

void Scheduler::setCurrentTime(float val) {
	currentTime = val;
}

StringList Scheduler::displaySchedule() {
	return "displaySchedule not implemented for this scheduler\n";
}

// Used if not overriden
const char* Scheduler::domain() const {
        return "Undefined";
}

// Interrupt handling stuff.  Currently, all interrupts that we catch
// are handled the same way.

void Scheduler::intCatcher(int) {
	interrupt = TRUE;
	return;
}

void Scheduler::catchInt(int signo, int always) {
	// unspecified interrupt means SIGINT.
	if (signo == -1) signo = SIGINT;
	if (!always) {
		// we don't catch signals if they are being ignored now
		SignalHandler tmp = signal(signo, SignalIgnore);
		if (tmp == SignalIgnore) return;
	}
	interrupt = FALSE;
	signal(signo, (SignalHandler)Scheduler::intCatcher);
}

void Scheduler::reportInterrupt() {
	Error::abortRun ("execution halted by user interrupt");
	interrupt = FALSE;
}
