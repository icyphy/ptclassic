#ifndef _CGDDFCluster_h
#define _CGDDFCluster_h 1

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1995 Seoul National University
All rights reserved.

 Programmer:  S. Ha

**************************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGMacroCluster.h"
#include "CGConScheduler.h"

class ostream;

class CGDDFClusterGal : public CGMacroClusterGal {
public:
	// contructor
	CGDDFClusterGal(Galaxy* g, ostream* l = 0) : CGMacroClusterGal(g,l) {}
	CGDDFClusterGal(CGMacroClusterBag* c, ostream* l = 0) :
		CGMacroClusterGal(c,l) {}

protected:
	CGMacroClusterBag* createGalBag(Galaxy* g);
};


class CGDDFClusterBag : public CGMacroClusterBag {
public:
	// constructor: makes an empty bag
	CGDDFClusterBag(Galaxy* g = 0): CGMacroClusterBag(g), conSched(0) {}
	~CGDDFClusterBag();

	// cluster inside galaxy
	/* virtual */ int clusterMyGal();

	// prepare myself
	/* virtual */ int prepareBag(MultiTarget* t, ostream* l);

	// parallel schedule
	/* virtual */ int parallelSchedule(MultiTarget* t);

	// compute the optimal number of the assigned processors
	/* virtual */ int computeProfile(int nP, int rW, IntArray* avail);

	// manual scheduling
	/* virtual */ Profile* manualSchedule(int);

	// inside scheduling
	/* virtual */ int finalSchedule(int numProcs);

	// average execution time
	/* virtual */ int myExecTime();

	// print my schedule
	StringList displaySchedule();

	// download code
	/* virtual */ void downLoadCode(CGStar*, int , int);

	// return fixNum
	int numFix();

private:
	int fixNum;	// set if the number of assigned processors 
			// are fixed. reset to 0 otherwise

        CGConScheduler* conSched;        // parallel scheduler

};

#endif
