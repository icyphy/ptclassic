/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/18/90
 Revisions:
	5/26/90 - I. Kuroda
		add InterpGalaxy::addState
		add InterpGalaxy::setState

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#ifndef _InterpGalaxy_h
#define _InterpGalaxy_h 1

#include "Galaxy.h"
#include "KnownBlock.h"
#include "KnownState.h"
#include "StringList.h"

class InterpGalaxy: public Galaxy {

private:
	StringList actionList;	// saves actions used in building galaxy
	PortHole* findPortHole(const char* star,const char* port);
public:
// constructor: makes an empty galaxy
	InterpGalaxy() { descriptor = "An interpreted galaxy";}

// set the descriptor
	void
	setDescriptor(const char* dtext) { descriptor = dtext;}

// add a new star to the galaxy
	int
	addStar(const char* starname, const char* starclass);

// add a connection
	int
	connect(const char* srcstar, const char* srcport, const char* dststar,
		const char* dstport, int delay = 0);
// add an alias
	int
	alias(const char* galport, const char* star, const char *starport);

// add a state
	int
        addState(const char* statename, const char* stateclass, const char* statevalue);

// set a state
	int
        setState(const char* blockname, const char* statename, const char* statevalue);

/// make portholes with a multiporthole
	int
	numPorts(const char* starname, const char* portname, int numP);

// Make a new, identical galaxy
	virtual Block *clone();

// "register" the galaxy (add it to the known list)
	void addToKnownList();

// return a contained star/galaxy (this is protected in Galaxy)
	Block* blockWithName(const char* s) {return Galaxy::blockWithName(s);}
};
#endif
