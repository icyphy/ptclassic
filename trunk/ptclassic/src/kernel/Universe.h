#ifndef _Universe_h
#define _Universe_h 1

// SCCS version identification
// $Id$

/*******************************************************************
	The generic Universe class.

	A Universe contains a Galaxy and a Scheduler.
	From the outside, it looks like a Star with no PortHoles.

********************************************************************/

	//////////////////////////////
	// Universe
	//////////////////////////////
	

#include "Galaxy.h"
#include "Scheduler.h"
#include "StringList.h"

class Universe : public Star {
public:
	// generate the schedule and/or initialize scheduler
	void initialize() {scheduler->setup(*myTopology);}

	// run the simulation
	void go() {scheduler->run(*myTopology);}

	// wrap up the simulation
	void wrapup() {scheduler->wrapup(*myTopology);}

	// set the stopping condition.  A hack.
	void setStopTime(float limit) {scheduler->setStopTime(limit);}

	// print methods
	StringList printVerbose() {return print(0);}
	StringList printRecursive() {return print(1);}

	// display schedule
	StringList displaySchedule() {return scheduler->displaySchedule();}

	// constructor
	Universe(Scheduler* s,const char* typeDesc, Galaxy* g) :
		scheduler(s), type(typeDesc), myTopology(g) {}

	// destructor
	virtual ~Universe() { delete scheduler; delete myTopology;}
protected:
	// print, possibly recursively
	StringList print(int recursive);
	// This should be called only once, with a reference to
	// the component galaxy specified.
	void addBlock(Block& g) {myTopology = (Galaxy*)&g;}
private:
	Scheduler* scheduler;
	Galaxy* myTopology;
	const char* type;
};
#endif
