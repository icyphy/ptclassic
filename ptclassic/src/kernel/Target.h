/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

class Target : public Block {
public:
	// Constructor
	Target(const char* nam, const char* starClass, const char* desc = "",
	       const char* assocDom = 0);

	// Destructor
	~Target();

	// Print the full name, descriptor, and state names and values
	/*virtual*/ StringList print(int verbose = 0) const;

	// Return the domain of the galaxy if it exists 
	/*virtual*/ const char* domain() const;

	// Invoke the begin methods of the constituent stars
	virtual void begin();

	// Invoke the go methods of the constituent stars
	virtual int run();

	// Invoke the wrapup methods of the constituent stars
	virtual void wrapup();

	// Return the starClass passed in the constructor 
	const char* starType() const { return starTypes.head(); }

	// Return the scheduler currently associated with this target
	Scheduler* scheduler() const { return sched; }

	// this function makes for cleaner code: clone() is derived
	// from Block and returns a duplicate obj so this cast is
	// always safe.  Redefine clone(), not cloneTarget!
	Target* cloneTarget() const { return (Target*)clone(); }

	// Ask the scheduler to display its schedule
	virtual StringList displaySchedule();

	// Return list of supported pragmas (annotations) as "name type value"
        // A Target may understand certain annotations in blocks called
	// "Pragmas".  This method returns the list of pragmas that a
	// particular target understands.  In derived classes, each item
	// in the list is a three part string, "name type value", separated
	// by spaces.  The default implementation returns a StringList with
        // only a single zero-length string in it.
	virtual StringList pragma () const { return ""; }

	// To determine the value of all pragmas that have been
	// specified for a particular block, call this method.
	// In derived classes, it returns a list of "name value"
	// pairs, separated by spaces.  In the base class, it
	// returns an empty string. The parentname is the name
	// of the parent class (universe or galaxy master name).
	virtual StringList pragma (const char* /*parentname*/,
				   const char* /*blockname*/) const {
	    return "";
	}

	// To determine the value of a pragma of a particular type
	// that has been specified for a particular block, call this
	// method. In derived classes, it returns a value.
	virtual StringList pragma (const char* /*parentname*/,
				   const char* /*blockname*/,
				   const char* /*pragmaname*/) const {
	    return "";
	}

	// To specify a pragma to a target, call this method.
	// The default implementation ignores all pragmas.
	// The return value is always a null string.
	virtual StringList pragma (const char* /*parentname*/,
				   const char* /*blockname*/,
				   const char* /*name*/,
				   const char* /*value*/) {
	    return "";
	}

	// return the nth child Target, null if no children.
	virtual Target* child(int n);

	// return the nth target, where I count as #0 if no children
	Target* proc(int n) {
		if (n == 0 && nChildren == 0) return this;
		else return child(n);
	}

	// return number of processors
	int nProcs() const { return children ? nChildren : 1; }

	// Does this target have the necessary resources to run the star?
	virtual int hasResourcesFor(Star& s, const char* extra = 0);

	// Does a child target have the necessary resource to run the star?
	virtual int childHasResources(Star& s, int childNum);

	// Initialize the target, given a galaxy. default: { gal = &g; }
	virtual void setGalaxy(Galaxy& g);

	// Zero the pointer to the current galaxy
        void clearGalaxy() { gal = 0; }

        // Set the stopping condition.
	virtual void setStopTime(double);

	// Reset the stopping condition (why different?)
	virtual void resetStopTime(double);

	// Set the current time
	virtual void setCurrentTime(double);

	// Routines for writing code: schedulers may call these
	// The depth normally just helps with indentations, but can
	// be used for other purposes.

	// Generate code to begin an iteration
	virtual void beginIteration(int repetitions, int depth);

	// Generate code to end an iteration
	virtual void endIteration(int repetitions, int depth);

	// Generate code for the star the way this target wants
	virtual void writeFiring(Star& s, int depth);

	// Generate code to initialize loops
	virtual void genLoopInit(Star& s, int reps);

	// Generate code to terminate loops
	virtual void genLoopEnd(Star& s);

	// Generate code to begin an if/then/else conditional construct
	virtual void beginIf(PortHole& cond, int truthdir, int depth,
			     int haveElsePart);

	// Generate code to continue an if/then/else conditional construct
	virtual void beginElse(int depth);

	// Generate code to end an if/then/else conditional construct
	virtual void endIf(int depth);

	// Generate code to begin a while conditional construct
	virtual void beginDoWhile(int depth);

	// Generate code to end a while conditional construct
	virtual void endDoWhile(PortHole& cond,int truthdir,int depth);

        // Resource management
        virtual int commTime(int sender,int receiver,int nUnits, int type);

	// Is the string a name of an ancestor of the current target?
	int isA(const char*) const;

	// Class identification
	const char* className() const;

	// Is the string a name of an ancestor of a child of the current target?
	virtual int childIsA(const char*) const;

	// Return the pointer to the current galaxy
	Galaxy* galaxy() const { return gal; }
    
	// Star classes supported
	StringList starTypes;
	
	// Do I support the given star type?
	virtual int support(Star* s); 

        // With these methods, we enable a scheduler or a target to
	// reset the target & galaxy description back to that stored
	// in the action list.  See InterpUniverse::initTarget and
	// InterpGalaxy::reset for more details
	void requestReset();
	int resetRequested();

protected:
	// Run galaxySetup and schedulerSetup
	virtual void setup();

	// Setup the galaxy, i.e. check star types and set target pointers
	virtual int galaxySetup();

	// Call setSched and compute the schedule
	virtual int schedulerSetup();

	// The following sets the scheduler and tells it which target belongs
	void setSched(Scheduler* sch);

	void delSched();

	// add a new child
	void addChild(Target&);

	// if the given target is one of my children, remove it from list
	// (does not delete the child!).  Return 1 if found in list, else 0.
	int remChild(Target&);

	// remove all my children (without deleting them)
	void remChildren();

	// delete all the "children" (for when they are created dynamically)
	void deleteChildren();

	// domain associated with the target
	const char* getAssociatedDomain() const { return associatedDomain; }

private:
        // reset target/galaxy flag
        int resetFlag;

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

	// domain associated with the parent target
	const char* associatedDomain;
};
#endif
