#ifndef _Universe_h
#define _Universe_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif


/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck

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
	int initSched() {
		return scheduler->setup(gal);
	}

	// run, until stopping condition
	void run() { scheduler->run(gal);}

	// end simulation
	void endSimulation() {wrapupGal(gal);}

	// set the stopping condition.  A hack.
	virtual void setStopTime(float stamp);

	// display schedule
	StringList displaySchedule() {return scheduler->displaySchedule();}

	// destructor: deletes scheduler
	~Runnable() { delete scheduler;}

protected:
	const char* type;
	Scheduler* scheduler;
	Galaxy& gal;
private:
	void wrapupGal (Galaxy& g);
};

	//////////////////////////////
	// Universe
	//////////////////////////////
	

class Universe : public Galaxy, public Runnable {
public:
	// print methods
	StringList printVerbose() const {return print(0);}
	StringList printRecursive() const {return print(1);}

	// constructor
	Universe(Scheduler* s,const char* typeDesc) :
		Runnable(s,typeDesc,this) {}

	// return my scheduler
	Scheduler* mySched() const { return scheduler ;}

	// class identification
	int isA(const char*) const;

protected:
	// print, possibly recursively
	StringList print(int recursive) const;
};
#endif
