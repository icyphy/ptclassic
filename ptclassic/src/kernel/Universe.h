#ifndef _Universe_h
#define _Universe_h 1

#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck

	The generic Universe class.

	A Universe is a Galaxy with a Target.
	From the outside, it looks like a Galaxy with no PortHoles.

********************************************************************/

#include "Galaxy.h"
#include "Target.h"
#include "StringList.h"

class Scheduler;

	//////////////////////////////
	// Runnable
	//////////////////////////////

class Runnable {
public:
	// constructor
	Runnable(Target* tar, const char* ty, Galaxy* g);

	// constructor to generate target from name
	Runnable(const char* targetname, const char* dom, Galaxy* g);

	// initialize and/or generate schedule
	void initTarget() {
		target->setGalaxy(*galP);
		target->initialize();
	}

	// run, until stopping condition
	int run() { return target->run();}

	// set the stopping condition.  A hack.
	virtual void setStopTime(double stamp);

	// display schedule
	StringList displaySchedule() {return target->displaySchedule();}

	// destructor: deletes scheduler

	virtual ~Runnable() { INC_LOG_DEL; delete target;}

protected:
	const char* type;
	Target* target;
	Galaxy* galP;
private:
	void wrapupGal (Galaxy& g);
};

	//////////////////////////////
	// Universe
	//////////////////////////////
	

class Universe : public Galaxy, public Runnable {
public:
	// print methods
	StringList print(int verbose = 0) const;

	// constructor
	Universe(Target* s,const char* typeDesc) :
		Runnable(s,typeDesc,this) {}

	// return my scheduler
	Scheduler* scheduler() const { return target->scheduler();}

	// class identification
	int isA(const char*) const;
	const char* className() const {return "Universe";}

	int run() { return Runnable::run();}
};
#endif
