#ifndef _Universe_h
#define _Universe_h 1

// SCCS version identification
// $Id$

/*******************************************************************
	The generic Universe class.

	A Universe is a Galaxy with a Scheduler.
	From the outside, it looks like a Galaxy with no PortHoles.
	(note that the portlist is still there because WormHoles,
	 inherited from Universe, can have PortHoles.
********************************************************************/

	//////////////////////////////
	// Universe
	//////////////////////////////
	

#include "Galaxy.h"
#include "Scheduler.h"
#include "StringList.h"

class Universe : virtual public Galaxy {
public:
	// generate the schedule and/or initialize scheduler
	void initialize() {
		initState();
		scheduler->setup(*this);
	}

	// run the simulation
	void go() {scheduler->run(*this);}

	// wrap up the simulation
	void wrapup() {scheduler->wrapup(*this);}

	// set the stopping condition.  A hack.
	void setStopTime(float limit) {scheduler->setStopTime(limit);}

	// print methods
	StringList printVerbose() {return print(0);}
	StringList printRecursive() {return print(1);}

	// display schedule
	StringList displaySchedule() {return scheduler->displaySchedule();}

	// constructor
	Universe(Scheduler* s,const char* typeDesc) :
		scheduler(s), type(typeDesc) {}

	// destructor.  Destroys scheduler.
	virtual ~Universe() { delete scheduler;}
protected:
	// print, possibly recursively
	StringList print(int recursive);

	const char* type;

	Scheduler* scheduler;
};
#endif
