#ifndef _Universe_h
#define _Universe_h 1

#include "type.h"
#include "Star.h"
#include "Scheduler.h"
#include "Galaxy.h"


// SCCS version identification
// $Id$

/*******************************************************************

	Universes are defined in this file.
	A Universe is a Star with a pointer to a component Galaxy,
	and a Scheduler to fire the blocks in the Galaxy.
	From the outside, it looks like a Star with no PortHoles.

	Currently, only an SDFUniverse is defined here.

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

	int initialize() {scheduler.setup(*myTopology);}
	int go() {scheduler.run(*myTopology);}
	int wrapup () {scheduler.wrapup(*myTopology);}

	// Redefine char* cast
	operator char* ();

	// Display the schedule
	char* displaySchedule() {scheduler.displaySchedule();}

protected:
	// The addBlock method should get invoked only once, with a
	// reference to the component galaxy specified.
	void addBlock(Block& b) {myTopology = &b;}
};

#endif
