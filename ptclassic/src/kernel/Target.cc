static const char file_id[] = "Target.cc";
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
#include "ConstIters.h"
#include "miscFuncs.h"
#include "Domain.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "miscFuncs.h"

void Target::setSched(Scheduler* sch) {
	LOG_DEL; delete sched;
	sched = sch;
	sch->setTarget(*this);
}

void Target::delSched() {
	LOG_DEL; delete sched;
	sched = 0;
}

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
					 "wrong star type for target, not a ",
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
	GalTopBlockIter next(*gal);
	Block* b;
	while ((b = next++) != 0) b->wrapup();
	return;
}

// by default, print to stdout
void Target::addCode (const char* code) {
	fputs (code, stdout);
}
		
int Target :: commTime(int,int,int,int) { return 0;}

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

// method for copying states during cloning.  It is designed for use
// by clone methods, and it assumes that the src argument has the same
// state list as me.  The idea is to write clone methods as

// MyTarget::clone() const { return (new MyTarget)->copyStates(*this);}

// It is protected to guard against abuse.

Target& Target::copyStates(const Target& src) {
	CBlockStateIter nexts(src);
	BlockStateIter nextd(*this);
	const State* srcStatePtr;
	State *destStatePtr;
	while ((srcStatePtr = nexts++) != 0 && (destStatePtr = nextd++) != 0)
		destStatePtr->setValue(srcStatePtr->getInitValue());
	return *this;
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
	const char* cname = hashstring(buf);
	newChild.setNameParent(cname,this);

	newChild.link = 0;
	nChildren++;
}

// inherit children
void Target::inheritChildren(Target* father, int start, int stop) {
	if (start < 0 || stop >= father->nProcs()) {
		Error::abortRun(readName(),
			"inheritance fails due to indices out of range");
		return;
	}
	children = father->child(start);
	nChildren = stop - start + 1;
}

// delete all children; use only when children were dynamically created.
void Target::deleteChildren() {
	Target* p = children;
	while (p) {
		children = p->link;
		LOG_DEL; delete p;
		p = children;
	}
	nChildren = 0;
}

char* Target::writeDirectoryName(const char* dirName) {

   if(dirName && *dirName) {
	const char* dir;

	// expand the path name
	dir = expandPathName(dirName);

	// check to see whether the directory exists, create it if not
	struct stat stbuf;
	if(stat(dir, &stbuf) == -1) {
	    // Directory does not exist.  Attempt to create it.
	    if (mkdir(dir, 0777)) {
		Error::warn("Cannot create Target directory : ", dir);
		return 0;
	    }
	} else {
	    // Something by that name exists, see whether it's a directory
	    if((stbuf.st_mode & S_IFMT) != S_IFDIR) {
		Error::warn("Target directory is not a directory: ",
					dirFullName);
		return 0;
	    }
	}
	// Directory is there
	return savestring(dir);
   } else return 0;
}

// Method to set a file name for writing.
// Prepends dirFullName to fileName with "/" between.
// Always returns a pointer to a string in new memory.
// It is up to the user to delete the memory when no longer needed.
// If dirFullName or fileName is NULL then it returns a
// pointer to a new copy of the string "/dev/null".
char* Target::writeFileName(const char* fileName) {
	if(dirFullName && *dirFullName && fileName && *fileName) {
		StringList fullName = dirFullName;
		fullName += "/";
		fullName += fileName;
		return fullName.newCopy();
	}
	return savestring("/dev/null");
}

// default functions to make event horizons
EventHorizon& Target :: newFrom() {
	Domain* tDomain = Domain::domainOf(*gal);
	return tDomain->newFrom();
}

EventHorizon& Target :: newTo() {
	Domain* tDomain = Domain::domainOf(*gal);
	return tDomain->newTo();
}

ISA_FUNC(Target,Block);
