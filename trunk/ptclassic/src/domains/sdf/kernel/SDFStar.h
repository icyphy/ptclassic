#ifndef _SDFStar_h
#define _SDFStar_h 1
#ifdef __GNUG__
#pragma once
#pragma interface
#endif

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90

 SDFStar is a Star that runs under the SDF Scheduler.
 Formerly defined in Star.h

*******************************************************************/

#include "Star.h"
#include "Fraction.h"
#include "SDFConnect.h"
#include "Particle.h"

class EGMaster;

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public Star  {
private:
        // pointer to master of instances for an expanded graph
	// TEMPORARY: these kinds of things don't belong here.
        EGMaster *master;

public:
	// constructor
	SDFStar() {
		repetitions = 0;
		noTimes = 0;
	}

	// my expanded graph master (temporary)
	EGMaster *myMaster() { return master;}

        // set the expanded graph master
        void setMaster(EGMaster *m) {master = m;}

	// my domain
	const char* domain() const;

	// domain specific initialization
	void prepareForScheduling();

	// define the firing 
	void fire();

	// The number of repetitions of the star in a periodic
	// schedule.  Initialized to 0 by constructor.  Set to correct
	// value by an SDF scheduler.  It is represented as a fraction
	// for the convenience of the scheduler.
	Fraction repetitions;

	// During scheduling, the scheduler must keep track of how
	// many times it has scheduled a star.  This is a convenient
	// place to do that.
	unsigned noTimes;

	// Execution time, for schedulers that use it
	virtual int myExecTime();
};

class SDFStarPortIter : public BlockPortIter {
public:
	SDFStarPortIter(SDFStar& s) : BlockPortIter(s) {}
	SDFPortHole* next() { return (SDFPortHole*)BlockPortIter::next();}
	SDFPortHole* operator++() { return next();}
};

#endif
