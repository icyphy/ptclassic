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
#pragma interface
#endif

#include "StringList.h"
#include "Block.h"

class Scheduler;
class Galaxy;
class EventHorizon;

class Target : public Block {
private:
	const char* supportedStarClass;
	Target* children;
	Target* link;
	int nChildren;
	Scheduler* sched;
protected:
	Galaxy* gal;

	// The following sets the scheduler and tells it which target belongs
	void setSched(Scheduler* sch);

	void delSched();

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

	// Method returns a directory name for writing.
	// If the directory does not exist, it attempts create it.
	// Always returns a pointer to a string in new memory, or 0
	// if it cannot find or create the directory.
	// It is up to the user to delete the string memory when
	// no longer needed.
	char* writeDirectoryName(const char* dirName = 0);

	// Method to set a file name for writing.
	// Prepends dirFullName to fileName with "/" between.
	// Always returns a pointer to a string in new memory.
	// It is up to the user to delete the memory when no longer needed.
	// If dirFullName or fileName is NULL then it returns a
	// pointer to a new copy of the string "/dev/null"
	char* writeFileName(const char* fileName = 0);

	// Store a directory name for writing
	char* dirFullName;
public:
	Target(const char* nam, const char* starClass,const char* desc = "") :
		Block(nam,0,desc),
		supportedStarClass(starClass), sched(0), gal(0),
		children(0), link(0), nChildren(0) {dirFullName = 0;}

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

	// run
	virtual int run();

	// wrapup
	virtual void wrapup();

	// Routines for writing code: schedulers may call these
	// The depth normally just helps with indentations, but can
	// be used for other purposes.

	// Function called to begin an iteration (default version
	// does nothing)
	virtual void beginIteration(int repetitions, int depth) {}

	// Function called to end an iteration (default version
	// does nothing)
	virtual void endIteration(int repetitions, int depth) {}

	// Function called to generate code for the star the way
	// this target wants (default version does nothing)

	virtual void writeFiring(Star& s, int depth) {}

        // resource management
        virtual int commTime(int sender,int receiver,int nUnits, int type);

	// class identification
	int isA(const char*) const;
	const char* readClassName() const {return "Target";}

	// functions to construct appropriate EventHorizon objects
	// for communication with the target.  Default implementations
	// forward to the Domain object for my galaxy's domain.
	virtual EventHorizon& newFrom();
	virtual EventHorizon& newTo();
};


#endif

