#ifndef _DEUniverse_h
#define _DEUniverse_h 1

#include "type.h"
#include "DEStar.h"
#include "DEScheduler.h"
#include "Galaxy.h"
#include "StringList.h"


// SCCS version identification
// $Id$

/*******************************************************************

	Programmer : Soonhoi Ha
	Description : DEUniverse is also defined here.
	Modification date : 5/30/90 - 

********************************************************************/

	//////////////////////////////
	// DEUniverse
	//////////////////////////////
	
class DEUniverse : public DEStar {
	DEScheduler scheduler;
public:
	// Temporarily make this public:  Maybe can be made private later
	// with scheduler as a friend.
	Block* myTopology;

	// setup the stopping condition
	void setStopTime(float limit) 
		{scheduler.setStopTime(limit) ;}

	void initialize() {scheduler.setup(*myTopology);}
	void go()
		{scheduler.run(*myTopology);}
	void wrapup () {scheduler.wrapup(*myTopology);}

	// Display the schedule
	StringList displaySchedule() 
		{return scheduler.displaySchedule(*myTopology);}

	// Print it out
	StringList printVerbose ();

	// Define clone() for convenience so Universe writers don't need to
	// Since it returns NULL, clone will fail unless redefined.
	Block* clone() { return NULL;}

protected:
	// The addBlock method should get invoked only once, with a
	// reference to the component galaxy specified.
	void addBlock(Block& b) {myTopology = &b;}
};

#endif
