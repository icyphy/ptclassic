#ifndef _BDFScheduler_h
#define _BDFScheduler_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

 The Boolean Data Flow Scheduler.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "BDFStar.h"
#include "BDFBool.h"
#include "SDFScheduler.h"

class BDFSchedule;
class BDFScheduler;

// This private class holds the scheduling information for a star.
class BDFStarSchedInfo {
	friend BDFScheduler;
	int numToksConst;	// if true, each arc always produces same
				// # of tokens (always true for SDF stars)
	BoolFraction reps;	// repetition count for star
	int set;		// flag indicating
	int noTimes;		// number of times run (simRunStar)
	int writesBoolean;	// writes a value used as a boolean
	BDFStarSchedInfo();
};

class SimGeo;

// This private class holds the scheduling information for a porthole.
class BDFPortSchedInfo {
	friend BDFScheduler;
	int num;		// number of tokens produced
	PortHole* assoc;	// associated boolean signal (0 if none)
	short relation;		// relation between signal and assoc
	unsigned char neg;	// if true, "real boolean" is complement
	unsigned char inFlag;	// if true, is input
	PortHole* finalAssoc;	// fully traced back associated boolean
				// (0 if none)
	int relDelay;		// relative delay
	SimGeo* geo;		// simulated geodesic
	int nmove;
	int lastRead;
	
	// constructor
	BDFPortSchedInfo();
	// destructor
	~BDFPortSchedInfo();
	// trace back porthole relationships
        void traceBack (DFPortHole&);
};

class BDFScheduler : public SDFScheduler {
public:
	// Constructor
	BDFScheduler() {
		starInfo = 0;
		portInfo = 0;
		dead = 0;
		galSize = 0;
		nPorts = 0;
		passValue = 0;
		mySchedule = 0;
		deferredStar = 0;
	}
	// Destructor
	~BDFScheduler();

	// setup and run are inherited from SDF.

	// runOnce does one iteration of the schedule.
	void runOnce();
	
	StringList displaySchedule();

	// return the number of repetitions for the indicated star.
	int reps(Star&);

	// save boolean values for a star
	void saveBooleans(Star& s);

	// retrieve boolean value for a star
	getBoolValue(const PortHole& p) const {
		return info(p).lastRead;
	}

protected:
	// prepare galaxy for scheduling
	int prepareGalaxy();

	// check stars for compatible domains
	int checkStars();

	// compute the BDF schedule (the biggie)
	int computeSchedule(Galaxy&);

private:
	// return info structure for a star
	BDFStarSchedInfo& info(const Star& s) const {
		return starInfo[s.index()];
	}

	// return info structure for a porthole
	BDFPortSchedInfo& info(const PortHole& p) const {
		return portInfo[p.index()];
	}

	// initialization functions for info structures.
	void initInfo (DataFlowStar&);
	void commonPortInfo (DFPortHole&);

	// compute # of tokens to be moved on each arc, based on booleans
	void computeTokens (const Star&, int useKnownBools = TRUE);

	// determine type of boolean token to write
	int valueToWrite (const PortHole&);

	// attempt to add a star to the schedule
	int addIfWeCan (Star& star, int defer = FALSE);

	// simulate the running of a star
	int simRunStar (Star& star, int defer = FALSE);

	// mark a star as deferred, if possible
	int deferIfWeCan (Star& star);

	// test to see if a star is runnable
	int notRunnable (Star& atom, int useKnownBools = TRUE);

	// compute repetitions for all stars
	int repetitions ();

	// subroutine of repetitions
	int reptConnectedSubgraph (Block&);

	// compute boolean expression for # of tokens on an arc
	BoolTerm numTok (const PortHole&);

	// apply repetition conditions for an arc
	int reptArc (PortHole&, PortHole&);

	// compensate for preamble to complete the schedule
	void subtractOffPreamble();

	// the main schedule: a pointer so we can hide its class.
	BDFSchedule *mySchedule;

	// the preamble to the schedule
	BlockList preamble;

	// if true, we are in the preamble
	int inPreamble;

	// least common multiple for denominators of BoolFraction expressions
	BoolTerm lcm;

	// error flag
	int galSize;		// number of stars
	int nPorts;		// number of star portholes

	// information structures for stars
	BDFStarSchedInfo* starInfo;

	// information structures for portholes
	BDFPortSchedInfo* portInfo;

	int passValue;
	int numDeferredStars;
	Star* deferredStar;
	Star* dead;

	// debug flag
	static int debug;
};

#endif
