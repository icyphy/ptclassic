static const char file_id[] = "Scheduler.cc";
/**************************************************************************
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

void Scheduler :: setGalaxy(Galaxy& g) {
	myGalaxy = &g;
	stopBeforeDeadFlag = FALSE;
	currentTime = 0;
}

// Return a StringList with code that can be executed to
// effect a run.  In the base class, this just causes an error.
void Scheduler :: compileRun() {
	Error :: abortRun ("This scheduler doesn't know how to compile.");
}

