/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 12/15/89

 Star is a Block that implements an atomic function...the
 Scheduler calls Stars at runtime to execute the simulation

*******************************************************************/
#ifndef _Star_h
#define _Star_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "Block.h"
class Galaxy;
class Target;

	////////////////////////////////////
	// class Star
	////////////////////////////////////

class Star : public Block  {

public:
// The writer of a Star code has the option to
// provide the methods start, go, and wrapup.
// Any Block, however, may have start and wrapup.

	// User supplied method.
	// Run the simulation, accessing Particles on the
	// input PortHoles, generating Particles on the output PortHoles
        virtual void go();

	// initialize domain-specific members.
	virtual void prepareForScheduling() = 0;

	// define firing 
	virtual void fire();

	// Method to print out description
	StringList printVerbose () const;

	// Return myself as a Star.  Overrides Block::asStar.
	Star& asStar();
	const Star& asStar() const;

	// Return my indexValue.
	int index() const { return indexValue;}

	// set the Target
	void setTarget(Target* t) { targetPtr = t;}

	// constructor
	Star() : targetPtr(0), indexValue(-1) {}

	// class identification
	int isA(const char*) const;
	const char* readClassName() const {return "Star";}
protected:
	Target* targetPtr;
private:
	int indexValue;
	friend setStarIndices(Galaxy&);
};

int setStarIndices(Galaxy&);
#endif
