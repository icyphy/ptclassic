#ifndef _SDFUniverse_h
#define _SDFUniverse_h 1

#include "type.h"
#include "SDFStar.h"
#include "SDFScheduler.h"
#include "Galaxy.h"
#include "StringList.h"


// SCCS version identification
// $Id$

/*******************************************************************
	This file used to be named Universe.h.

	An SDF Universe is an SDF Star with a pointer to a component Galaxy,
	and an SDF Scheduler to fire the blocks in the Galaxy.
	From the outside, it looks like a Star with no PortHoles.

********************************************************************/

	//////////////////////////////
	// SDFUniverse
	//////////////////////////////
	
class SDFUniverse : public SDFStar {
	SDFScheduler scheduler;
public:
	// Temporarily make this public:  Maybe can be made private later
	// with scheduler as a friend.
	Block* myTopology;

	void initialize() {scheduler.setup(*myTopology);}
	void go() {scheduler.run(*myTopology);}
	void wrapup () {scheduler.wrapup(*myTopology);}

	// The "full dump"
	StringList printVerbose ();

	// Display the schedule
	StringList displaySchedule() {return scheduler.displaySchedule();}

	// Define clone() for convenience so Universe writers don't need to
	// Since it returns NULL, clone will fail unless redefined.
	Block* clone() { return NULL;}
protected:
	// The addBlock method should get invoked only once, with a
	// reference to the component galaxy specified.
	void addBlock(Block& b) {myTopology = &b;}
};

#endif
