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
public:
	Target(const char* nam, const char* starClass,const char* desc = "");
	~Target();

	/* virtual */ StringList print(int verbose = 0) const;

	const char* starType() const { return supportedStarClass;}

	Scheduler* scheduler() const { return sched;}

	// this function makes for cleaner code: clone() is derived
	// from Block and returns a duplicate obj so this cast is
	// always safe.  Redefine clone(), not cloneTarget!

	Target* cloneTarget() const { return (Target*)clone();}

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

	// determine whether this target has the necessary resources
	// to run the given star.  virtual in case later necessary.
	// default implemenation uses "resources" states of the target
	// and the star.
	virtual int hasResourcesFor(Star& s,const char* extra=0);

	// determine whether a particular child target has resources
	// to run the given star.  virtual in case later necessary.
	virtual int childHasResources(Star& s,int childNum);

	// initialize the target, given a galaxy.
	virtual void setGalaxy(Galaxy& g); // default: { gal = &g; }

	// set the stopping condition.
	virtual void setStopTime(double);

	// reset the stopping condition (why different?)
	virtual void resetStopTime(double);

	// set the current time
	virtual void setCurrentTime(double);

	// run
	virtual int run();

	// wrapup
	virtual void wrapup();

	// Routines for writing code: schedulers may call these
	// The depth normally just helps with indentations, but can
	// be used for other purposes.

	// Function called to begin an iteration (default version
	// does nothing)
	virtual void beginIteration(int repetitions, int depth);

	// Function called to end an iteration (default version
	// does nothing)
	virtual void endIteration(int repetitions, int depth);

	// Function called to generate code for the star the way
	// this target wants (default version does nothing)

	virtual void writeFiring(Star& s, int depth);

	// Functions to generate loop initialization and termination code
	// (default versions do nothing)
	virtual void genLoopInit(Star& s, int reps);
	virtual void genLoopEnd(Star& s);

	// Functions for generation of conditional constructs
	// default versions do nothing
	virtual void beginIf(PortHole& cond,int truthdir,int depth,int haveElsePart);
	virtual void beginElse(int depth);
	virtual void endIf(int depth);
	virtual void beginDoWhile(int depth);
	virtual void endDoWhile(PortHole& cond,int truthdir,int depth);

        // resource management
        virtual int commTime(int sender,int receiver,int nUnits, int type);

	// class identification
	int isA(const char*) const;
	const char* className() const;
	Galaxy* galaxy() { return gal;}
protected:

	// initialization for the target (called by initialize)
	virtual void setup();

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

	// auxililary star class: permits a second type of star in addition
	// to supportedStarClass.
	virtual const char* auxStarClass() const;

	// Method returns a directory name for writing.
	// If the directory does not exist, it attempts create it.
	// Returns the fully expanded pathname (which is saved by
	// the target).

	const char* writeDirectoryName(const char* dirName = 0);

	// this fn returns it.
	const char* workingDirectory() const { return dirFullName;}

	// Method to set a file name for writing.
	// Prepends dirFullName (which was set by writeDirectoryName)
	// to fileName with "/" between.
	// Always returns a pointer to a string in new memory.
	// It is up to the user to delete the memory when no longer needed.
	// If dirFullName or fileName is NULL then it returns a
	// pointer to a new copy of the string "/dev/null"
	char* writeFileName(const char* fileName = 0);

private:
	// name of class of star this target supports
	const char* supportedStarClass;
	// points to a linked list of children
	Target* children;
	// points to the next sibling (child of same parent)
	Target* link;
	// number of children
	int nChildren;
	// scheduler for the target
	Scheduler* sched;
	// galaxy of stars to be run on the target
	Galaxy* gal;
	// Store a directory name for writing
	char* dirFullName;
};
#endif
