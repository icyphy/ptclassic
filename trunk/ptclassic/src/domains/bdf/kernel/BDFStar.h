#ifndef _BDFStar_h
#define _BDFStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

 BDFStar is a Star that runs under the BDF Scheduler.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "BDFConnect.h"
#include "Particle.h"

	////////////////////////////////////
	// class BDFStar
	////////////////////////////////////

class BDFStar : public DataFlowStar  {

public:
	// my domain
	const char* domain() const;

	// domain specific initialization
	void prepareForScheduling();

	// functions for schedule generation
	int notRunnable();
	int simRunStar(int deferFiring);
	int deferrable();
	// identification
	int isSDF() const;
	int isA(const char*) const;
};

class BDFStarPortIter : public BlockPortIter {
public:
	BDFStarPortIter(BDFStar& s) : BlockPortIter(s) {}
	BDFPortHole* next() { return (BDFPortHole*)BlockPortIter::next();}
	BDFPortHole* operator++() { return next();}
};

// I want to put the following into SDFStar.h eventually.

class DFStarPortIter : public BlockPortIter {
public:
	DFStarPortIter(DataFlowStar& s) 
	: BlockPortIter(s) {}
	DFPortHole* next() { return (DFPortHole*)BlockPortIter::next();}
	DFPortHole* operator++() { return next();}
};

#endif
