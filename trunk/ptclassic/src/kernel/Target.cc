static const char file_id[] = "Target.cc";
/*******************************************************************
SCCS version identification
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
#include <sys/types.h>
#include <sys/stat.h>
#include "SimControl.h"
#include "StringArrayState.h"

// constructor
Target::Target(const char* nam, const char* starClass,const char* desc) :
Block(nam,0,desc), supportedStarClass(starClass), sched(0), gal(0),
children(0), link(0), nChildren(0), dirFullName(0) {}

// destructor
Target::~Target() {
// should we delete children?
	LOG_DEL; delete dirFullName;
}

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

// default auxiliary star class: none.
const char* Target::auxStarClass() const { return "";}

void Target::setGalaxy(Galaxy& g) {
	gal = &g;
}

// default setup: set all stars to have me as their Target, then
// do Scheduler::setup.

void Target::setup() {
	if (gal == 0) {
		Error::abortRun(*this, "Error in Target::setup() -- ",
				" no galaxy attached to the target");
		return;
	}
	GalStarIter next(*gal);
	Star* s;
	while ((s = next++) != 0) {
		if (!s->isA(supportedStarClass) && 
		    !s->isA(auxStarClass())) {
			Error::abortRun (*s,
					 "wrong star type for target ",
					 name());
			return;
		}
		s->setTarget(this);
	}
	if (!sched) {
		Error::abortRun(*this, "No scheduler!");
		return;
	}
	sched->setGalaxy(*gal);
	sched->setup();
}

// default run: run the scheduler
int Target::run() {
	return sched->run();
}

// default wrapup: call wrapup on all stars and galaxies
void Target::wrapup () {
	if (!gal) return;
	GalTopBlockIter next(*gal);
	Block* b;
	while ((b = next++) != 0) b->wrapup();
	return;
}

int Target :: commTime(int,int,int,int) { return 0;}

// by default, pass these on through
void Target::setStopTime(double f) { sched->setStopTime(f);}
void Target::resetStopTime(double f) { sched->resetStopTime(f);}
void Target::setCurrentTime(double f) { sched->setCurrentTime(f);}

StringList Target::print(int verbose) const {
	StringList out;
	out << "Target: " << fullName() << "\n";
	out << "Descriptor: " << descriptor() << "\n";
	out << printStates("target",verbose);
	if (nChildren > 0) {
		out << "The target has " << nChildren << " children\n";
	}
	return out;
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

// Create child name.
	StringList childName = "proc";
	childName << nChildren;
	const char* cname = hashstring(childName);
	newChild.setNameParent(cname,this);

	newChild.link = 0;
	nChildren++;
}

// inherit children
void Target::inheritChildren(Target* father, int start, int stop) {
	if (start < 0 || stop >= father->nProcs()) {
		Error::abortRun(name(),
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

// function for use by "hasResourcesFor".  Finds the StringArrayState
// named "resources" in the given Block, if any.
static StringArrayState* getResourceState(Block& b) {
	State* s = b.stateWithName("resources");
	if (!s || !s->isA("StringArrayState")) return 0;
	return (StringArrayState*)s;
}

// determine whether this target satisfies the resource requirements
// of the star s.

int Target::hasResourcesFor(Star& s,const char* extra) {
	// if the star requests no resources, then we have all the
	// resources the star needs.

	StringArrayState* stR = getResourceState(s);
	if (stR == 0) return TRUE;

	StringArrayState* tR = getResourceState(*this);

	int stSz = stR->size();
	StringArrayState& stateRes = *stR;
	// check that all resources are provided by the target.  The
	// target provides resources either through its resources state,
	// the names of its classes and baseclasses, and "extra", which
	// provides "2" in child target 2, for example.
	// we return FALSE if any resource is not supplied.
	for (int i = 0; i < stSz; i++) {
		const char* res = stateRes[i];
		if ((tR && tR->member(res)) || isA(res)) continue;
		if (extra && strcmp(extra,res) == 0) continue;
		return FALSE;
	}
	return TRUE;
}

// determine whether a particular child target of this Target
// has resources to run the given star.

int Target::childHasResources(Star& s,int childNum) {
	Target* ch = proc(childNum);
	if (ch == 0) return FALSE;
	StringList num(childNum);
	return ch->hasResourcesFor(s,num);
}
      
// set up directory for writing files associated with Target.

const char* Target::writeDirectoryName(const char* dirName) {

   if(dirName && *dirName) {
	// expand the path name
	dirFullName = savestring(expandPathName(dirName));

	// check to see whether the directory exists, create it if not
	struct stat stbuf;
	if(stat(dirFullName, &stbuf) == -1) {
	    // Directory does not exist.  Attempt to create it.
	    if (mkdir(dirFullName, 0777)) {
		Error::warn("Cannot create Target directory : ", dirFullName);
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
	return dirFullName;
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

// Small virtual functions
void Target::beginIteration(int,int) {}
void Target::endIteration(int,int) {}
void Target::writeFiring(Star&,int) {}
const char* Target::className() const {return "Target";}
ISA_FUNC(Target,Block);
