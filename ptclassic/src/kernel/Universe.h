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

	
#include "Galaxy.h"
#include "Scheduler.h"
#include "StringList.h"

	//////////////////////////////
	// Runnable
	//////////////////////////////

class Runnable {
public:
	// constructor
	Runnable(Scheduler* s, const char* t, Galaxy* g) :
		type(t), scheduler(s), gal(*g) {}

	// initialize and/or generate schedule
	void initSched() {
		gal.initState();
		scheduler->setup(gal);
	}

	// run, until stopping condition
	void run() { scheduler->run(gal);}

	// wrapup simulation
	void wrapup() {scheduler->wrapup(gal);}

	// set the stopping condition.  A hack.
	void setStopTime(float limit) {scheduler->setStopTime(limit);}

	// display schedule
	StringList displaySchedule() {return scheduler->displaySchedule();}

	// destructor: deletes scheduler
	~Runnable() { delete scheduler;}

protected:
	const char* type;
	Scheduler* scheduler;
	Galaxy& gal;
};

	//////////////////////////////
	// Universe
	//////////////////////////////
	

class Universe : public Galaxy, public Runnable {
public:
	// print methods
	StringList printVerbose() {return print(0);}
	StringList printRecursive() {return print(1);}

	// constructor
	Universe(Scheduler* s,const char* typeDesc) :
		Runnable(s,typeDesc,this) {}

protected:
	// print, possibly recursively
	StringList print(int recursive);
};
#endif
