#ifndef _Universe_h
#define _Universe_h 1

#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
SCCS version identification
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
