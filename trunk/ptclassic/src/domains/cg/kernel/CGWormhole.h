#ifndef _CGWormhole_h
#define _CGWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "Wormhole.h"
#include "Profile.h"
#include "MultiTarget.h"
#include "EventHorizon.h"
#include "CGPortHole.h"

/*******************************************************************
 SCCS Version identification :
 $Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer : Soonhoi Ha
 Date of Creation : 5/21/91
	
********************************************************************/

// CGWormhole can not contain a timed-domain. It may contain another
// type of CGdomain.

	//////////////////////////////
	// CGWormhole
	//////////////////////////////

class CGWormhole : public Wormhole, public CGStar {

private:
        // The total number of processors  in the system.
        int numProcs;
	int prevNum;

	// The final optimal number of assigned processors to inside
	// CG domain.
	int optNum;

        // execution profile of the wormhole, presumably a dynamic construct.
        // profiles for varying number of assigned processors.
        Profile* profile;

	// multi target indicator
	MultiTarget* mtarget;

	// Domains to be supported
	StringList supportedDomains;

	// execution time
	int execTime;

public:
	void setup();
		       
	void go();
	void wrapup() { target->wrapup(); }

	// Constructor
	CGWormhole(Galaxy& g, Target* t = 0);
	~CGWormhole();

	// return my scheduler
	Scheduler* scheduler() const { return target->scheduler() ;}

	// execution time which is the average of the workload inside 
	// the wormhole with 1 processor.
	int myExecTime() { return execTime; }

	// display the schedules of the CGDDFWormholes.
	StringList displaySchedule();

	// print methods
	StringList print(int verbose = 0) const {
		return Wormhole::print(verbose);
	}

	// clone -- allows interpreter/pigi to make copies
	Block* clone() const;

	// identify myself as a wormhole
	int isItWormhole() const { return TRUE;}

	// use statelist for inner galaxy for stateWithName
	State* stateWithName (const char* name) {
		return gal.stateWithName(name);
	}
	
	// state initialize
	void initState() { gal.initState() ;}

	// return myself
	CGWormhole* myWormhole();

	// FIXME: what should this do?
	double getStopTime() { return 0.0;}

/*******  methods for parallel scheduler. ********/

        // return the profile when "pNum" processors are assigned.
        Profile& getProfile(int pNum) { pNum--; return profile[pNum]; }

        // set the "numProcs" member and initialize Profiles.
        void assignProcs(int num);

        // compute the optimal profile with the given number of processors.
        void computeProfile(int num, int resWork, IntArray* avail);

	// main schedule
	void insideSchedule();

	// down-load the code
	void downLoadCode(int);

};


        //////////////////////////////////////////
        // class CGtoUniversal
        //////////////////////////////////////////

class CGtoUniversal : public ToEventHorizon, public InCGPort
{
public:
	// constructor
	CGtoUniversal(): ToEventHorizon(this) {}

	// redefine
	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as Eventhorizon
	EventHorizon* asEH();
};

        //////////////////////////////////////////
        // class CGfromUniversal
        //////////////////////////////////////////

class CGfromUniversal : public FromEventHorizon, public OutCGPort
{
public:
	// constructor
	CGfromUniversal(): FromEventHorizon(this) {}

	// redefine
	void initialize();

	int isItInput() const;
	int isItOutput() const;

	// as Eventhorizon
	EventHorizon* asEH();
};

#endif
