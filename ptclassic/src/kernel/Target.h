/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 9/25/91

 A Target is an object that controls execution.  It contains a Scheduler,
 for determining order of execution.

**************************************************************************/

#ifndef _Target_h
#define _Target_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "StringList.h"
#include "Block.h"

class Scheduler;
class Galaxy;
class PGNode;
class Cluster;
class PGParSchedule;
class IntList;

class Target : public Block {
private:
	const char* supportedStarClass;
	Target* children;
	Target* link;
	int nChildren;
protected:
	Scheduler* sched;
	Galaxy* gal;

	void setSched(Scheduler* sch) { sched = sch;}

	// add a new child
	void addChild(Target&);

	// add a inherited child
	void inheritChildren(Target*, int, int);

	// remove the "children" list (no effect on children)
	void remChildren() { nChildren = 0; children = 0;}

	// delete all the "children" (for when they are created dynamically)
	void deleteChildren();

// method for copying states during cloning.  It is designed for use
// by clone methods, and it assumes that the src argument has the same
// state list as me.  The idea is to write clone methods as

// MyTarget::clone() const { return (new MyTarget)->copyStates(*this);}
	Target& copyStates(const Target&);
public:
	Target(const char* nam, const char* starClass,const char* desc = "") :
		Block(nam,0,desc),
		supportedStarClass(starClass), sched(0), gal(0),
		children(0), link(0), nChildren(0) {}

	StringList printVerbose() const;

	const char* starType() const { return supportedStarClass;}

	Scheduler* mySched() const { return sched;}

	// this function makes for cleaner code: clone() is derived
	// from Block and returns a duplicate obj so this cast is
	// always safe.  Redefine clone(), not cloneTarget!

	Target* cloneTarget() const { return (Target*)clone();}

	// IMPORTANT: when redefining clone() for a target with states,
	// use copyStates as shown above.

	// send a string to the Target
	virtual void addCode (const char* code);

	virtual StringList displaySchedule();

	virtual int commTime(int sender,int receiver,int nUnits, int type);

	// return the nth child Target, null if no children.
	Target* child(int n);

	// return the nth target, where I count as #0 if no children
	Target* proc(int n) {
		if (n == 0 && nChildren == 0)
			return this;
		else return child(n);
	}

	// return number of processors
	int nProcs() const { return children ? nChildren : 1;}

	// initialize the target, given a galaxy.
	virtual int setup(Galaxy&);

	// set the stopping condition.
	virtual void setStopTime(float);

	// reset the stopping condition (why different?)
	virtual void resetStopTime(float);

	// set the current time
	virtual void setCurrentTime(float);

	// initialize data structures
	virtual void initialize();

	// NOTE: to have the Target do specific things based on
	// state values, do it in a start() function, as for stars.

	// run
	virtual int run();

	// wrapup
	virtual void wrapup();

	// methods for use in parallel scheduling -- the baseclass
	// takes appropriate actions for a single processor, which
	// is usually almost nothing.

	// these functions manipulate the saved communication pattern
	// (which doesn't exist in the baseclass)
	virtual void clearCommPattern();
	virtual void saveCommPattern();
	virtual void restoreCommPattern();

	virtual PGNode* backComm(PGNode*);

	// this function schedules a node
	virtual int scheduleComm(PGNode*, int earliestStart);

	// this function returns a list of alternative candidate processors
	virtual IntList* whichProcs(Cluster*, PGParSchedule*);

};


#endif

