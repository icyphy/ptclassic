/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck

	Target

	A Target represents an object that receives code to be
	run and executes it, for example.

********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Target.h"
#include "Scheduler.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Error.h"
#include <stdio.h>

// default commTime method: only one processor, no time

int Target::commTime(int,int,int,int) { return 0;}

// display the schedule
StringList Target::displaySchedule() {
	if (sched) {
		return sched->displaySchedule();
	}
	return "no scheduler member, so no schedule\n";
}

// default setup: set all stars to have me as their Target, then
// do Scheduler::setup.

int Target::setup(Galaxy& g) {
	gal = &g;
	GalStarIter next(g);
	Star* s;
	while ((s = next++) != 0) {
		if (!s->isA(supportedStarClass)) {
			Error::abortRun (*s,
					 "wrong star type for target, not a",
					 supportedStarClass);
			return FALSE;
		}
		s->setTarget(this);
	}
	if (!sched) {
		Error::abortRun(*this, "No scheduler!");
		return FALSE;
	}
	sched->stopBeforeDeadlocked = FALSE;
	return sched->setup(g);
}

// default run: run the scheduler
int Target::run() {
	return sched->run(*gal);
}

// default wrapup: call wrapup on all stars and galaxies
void Target::wrapup () {
	if (!gal) return;
	GalAllBlockIter next(*gal);
	Block* b;
	while ((b = next++) != 0) b->wrapup();
	return;
}

// by default, print to stdout
void Target::addCode (const char* code) {
	fputs (code, stdout);
}
		
// by default, pass these on through
void Target::setStopTime(float f) { sched->setStopTime(f);}
void Target::resetStopTime(float f) { sched->resetStopTime(f);}
void Target::setCurrentTime(float f) { sched->setCurrentTime(f);}

StringList Target::printVerbose() const {
	StringList out;
	out = "Target: ";
	out += readFullName();
	out += "\n";
	out += "Descriptor: ";
	out += readDescriptor();
	out += "\n";
	out += printStates("target");
	if (nChildren > 0) {
		out += "The target has ";
		out += nChildren;
		out += " children\n";
	}
	return out;
}

void Target::initialize() {
	Block::initialize();
}

// method for copying states during cloning.  It is designed for use
// by clone methods, and it assumes that the src argument has the same
// state list as me.  The idea is to write clone methods as

// MyTarget::clone() const { return (new MyTarget)->copyStates(*this);}

// It is protected to guard against abuse.

Target& Target::copyStates(const Target& src) {
	BlockStateIter nexts(src);
	BlockStateIter nextd(*this);
	State *srcStatePtr, *destStatePtr;
	while ((srcStatePtr = nexts++) != 0 && (destStatePtr = nextd++) != 0)
		destStatePtr->setValue(srcStatePtr->getInitValue());
	return *this;
}

// Dummy parallel scheduling funcs.
void Target :: clearCommPattern() {}
void Target :: saveCommPattern() {}
void Target :: restoreCommPattern() {}

PGNode* Target :: backComm(PGNode*) { return 0;}

	// this function schedules a node
int Target :: scheduleComm(PGNode*, int earliestStart) {
	return earliestStart;
}

// this function returns a list of alternative candidate processors
IntList* Target :: whichProcs(Cluster*, PGParSchedule*) {
	return 0;
}

// return the nth child.
Target* Target::child(int n) {
	Target*p = children;
	while (n > 0 && p) {
		p = p->link;
		n--;
	}
	return p;
}

// add a new child target.
void Target::addChild(Target& newChild) {
	if (children == 0) {
		children = &newChild;
	}
	else {
		// attach new child to the end of the chain.
		Target*p = children;
		while (p->link) p = p->link;
		p->link = &newChild;
	}

// Create child name.   This is a memory leak!
	char buf[20];
	sprintf (buf, "proc%d", nChildren);
	char* cname = savestring(buf);
	newChild.setNameParent(cname,this);

	newChild.link = 0;
	nChildren++;
}

// delete all children; use only when children were dynamically created.
void Target::deleteChildren() {
	Target* p = children;
	while (p) {
		children = p->link;
		delete p;
		p = children;
	}
	nChildren = 0;
}
