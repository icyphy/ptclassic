static const char file_id[] = "Target.cc";
/*******************************************************************
SCCS version identification
@(#)Target.cc	2.60     07/21/97

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
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
#include "Domain.h"
#include "SimControl.h"
#include "StringArrayState.h"

// constructor
Target::Target(const char* nam, const char* starClass,
	       const char* desc, const char* assocDomain) :
Block(nam,0,desc), resetFlag(0), children(0), link(0), nChildren(0),
sched(0), gal(0), associatedDomain(assocDomain) {
	starTypes = starClass;
}

// destructor
Target::~Target() {
	// Unlink myself from my parent
	if (parent() && parent()->isA("Target")) {
		((Target*)parent())->remChild(*this);
	}
	// Unlink myself from my children
	remChildren();
	// FIXME: Should we delete the children, rather than just unlinking?
	// Seems safer to expect derived classes to call deleteChildren
	// in their destructors if they dynamically allocated their children.
	// If they forget, memory leak, but no crash.
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
	return "{ { scheduler \"no scheduler member, so no schedule\"} }";
}

void Target::setGalaxy(Galaxy& g) {
	gal = &g;
}

// With these methods, we enable a scheduler or a target to
// reset the target & galaxy description back to that stored
// in the action list.  See InterpUniverse::initTarget and
// InterpGalaxy::reset for more details
void Target::requestReset() {
	// Handle case of being a child target
	if (parent() && parent()->isA("Target")) {
		((Target*)parent())->requestReset();
	}

	// Handle case of being inside a wormhole
	if (galaxy() && galaxy()->parent() &&
	    galaxy()->parent()->isItWormhole() &&
	    galaxy()->parent()->target()) {
		galaxy()->parent()->target()->requestReset();
	}

	resetFlag = TRUE;
}

int Target::resetRequested() {
    return resetFlag;
}

// default setup: set all stars to have me as their Target, then
// do Scheduler::setup.

void Target::setup() {
	if (!galaxySetup()) return;
	else if (!schedulerSetup()) return;
	else return;
}

// setup the galaxy, i.e. check star types and set targets pointers
int Target::galaxySetup() {
	if (! gal) {
		Error::abortRun(*this, "Error in Target::setup() -- ",
				" no galaxy attached to the target");
		return FALSE;
	}

	return gal->setTarget(this);
}

// do I support a given star
int Target :: support(Star* star) {
    // First check for target supported stars
    int supportFlag = FALSE;
    StringListIter types(starTypes);
    const char* starType = 0;
    while (!supportFlag && ((starType = types++) != 0))
	supportFlag = star->isA(starType);

    // Now check the sub-domain supported stars
    if (!supportFlag && galaxy()) {
	supportFlag = (strcmp(galaxy()->domain(), star->domain()) == 0);
	Domain* dom = Domain::of(*galaxy());
	if(! dom) {
	    Error::abortRun(*galaxy(),
			    "Cannot figure out the domain of the galaxy");
	    return FALSE;
	}
	StringListIter subdomains(dom->subDomains);
	const char* sub = 0;
	while (!supportFlag && ((sub = subdomains++) != 0))
	    supportFlag = (strcmp(sub, star->domain()) == 0);
    }
    return supportFlag;
}

// setup the scheduler
int Target::schedulerSetup() {
	if (!sched) {
		Error::abortRun(*this, "No scheduler!");
		return FALSE;
	}
	if (!gal) {
		Error::abortRun(*this, "No galaxy!");
		return FALSE;
	}
	sched->setGalaxy(*gal);
	sched->setup();
	if (SimControl::haltRequested()) return FALSE;
	return TRUE;
}

// invoke begin methods
void Target::begin() {
   	if (!gal) return;

	GalStarIter nextStar(*gal);
	Star *s;
	while ((s = nextStar++) != 0) {
	    s->begin();
	    if (SimControl::haltRequested()) return;
	}

	// resetPortBuffers() in each Block, serving as a reinitialize (but
	// still keep the data structure).
	gal->resetPortBuffers();

	// If we have child targets - invoke begin methods
	if (!nChildren) return;

	int i;
	for (i = 0; i < nProcs() ; i++) {
	    child(i)->begin();
	    if (SimControl::haltRequested()) return;
	}
}

// default run: run the scheduler
int Target::run() {
	if (sched) return sched->run();

	Error::abortRun(*this, "No scheduler!");
	return FALSE;
}

// default wrapup: call wrapup on the galaxy
void Target::wrapup () {
	if (!gal) return;
	// Note: we can't simply say "gal->wrapup()" below because 
	// wrapup is a virtual method and if gal points to an 
	// InterpUniverse then we will get into infinite recursion.
	gal->Galaxy::wrapup();
}

int Target::commTime(int,int,int,int) { return 0;}

// by default, pass these on through
void Target::setStopTime(double f) { if (sched) sched->setStopTime(f);}
void Target::resetStopTime(double f) { if (sched) sched->resetStopTime(f);}
void Target::setCurrentTime(double f) { if (sched) sched->setCurrentTime(f);}

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
	// safety check: if it's already in some child chain, take it out
	// to avoid corrupting that chain.
	if (newChild.parent() && newChild.parent()->isA("Target")) {
		((Target*)newChild.parent())->remChild(newChild);
	}

	if (children == 0) {
		children = &newChild;
	}
	else {
		// attach new child to the end of the chain.
		Target*p = children;
		while (p->link) p = p->link;
		p->link = &newChild;
	}

	newChild.setParent(this);

	newChild.link = 0;
	nChildren++;
}

// if the given target is one of my children, remove it from list
// (does not delete the child!).  Return 1 if found in list, else 0.
int Target::remChild(Target& child) {
	if (&child == children) {
		children = child.link;
		nChildren--;
		child.setParent(0);
		return TRUE;
	}
	for (Target* p = children; p; p = p->link) {
		if (&child == p->link) {
			p->link = child.link;
			nChildren--;
			child.setParent(0);
			return TRUE;
		}
	}
	return FALSE;
}

// remove all my children (without deleting them)
void Target::remChildren()
{
	for (Target* p = children; p; p = p->link) {
		p->setParent(0); // we needn't bother clearing p->link
	}
	children = 0;
	nChildren = 0;
}

// delete all children; use only when children were dynamically created.
void Target::deleteChildren() {
	Target* p = children;
	while (p) {
		// NOTE: each child is removed from list before being deleted,
		// but we do not clear its parent link.  A derived Target
		// destructor might want to know whether it is a child target,
		// so clearing the parent link would be bad.  But since we
		// already unlinked, Target::~Target's attempt to remChild
		// will fail harmlessly.
		children = p->link;
		nChildren--;
		LOG_DEL; delete p;
		p = children;
	}
	nChildren = 0;		// just to be sure
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
      
// Small virtual functions with dummy implementations
void Target::beginIteration(int,int) {}
void Target::endIteration(int,int) {}
void Target::writeFiring(Star&,int) {}
void Target::genLoopInit(Star&,int) {}
void Target::genLoopEnd(Star&) {}
void Target::beginIf(PortHole&,int,int,int) {}
void Target::beginElse(int) {}
void Target::endIf(int) {}
void Target::beginDoWhile(int) {}
void Target::endDoWhile(PortHole&,int,int) {}

const char* Target::className() const {return "Target";}
ISA_FUNC(Target,Block);

// see if there is a child of the appropriate type
int Target::childIsA(const char* type) const {
	Target * c = children;
	while (c) {
		if (c->isA(type)) return TRUE;
		c = c->link;
	}
	return FALSE;
}

// Return the domain of the galaxy if it exists or
// the associated domain if it is non-null or
// the value of the baseclass domain method otherwise
const char* Target::domain() const {
	if (galaxy()) return galaxy()->domain();
	else if (associatedDomain) return associatedDomain;
	else return Block::domain();
}

// For a target within a Wormhole, if this method returns TRUE
// and the outside domain is timed, then the target will be
// fired again at the time returned by the nextFiringTime method.
// In this base class, always return FALSE.
int Target::selfFiringRequested() {
return FALSE;
}

// If selfFiringRequested returns TRUE, return the time at which
// this firing is requested.
double Target::nextFiringTime() {
return 0.0;
}
