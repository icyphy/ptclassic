/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 3/18/90

 InterpGalaxy is a galaxy that may dynamically be added to, as well
 as cloned.

*******************************************************************/
#ifndef _InterpGalaxy_h
#define _InterpGalaxy_h 1

#include "Galaxy.h"
#include "KnownBlock.h"
#include "StringList.h"

class InterpGalaxy: public Galaxy {

private:
	StringList actionList;	// saves actions used in building galaxy
	PortHole *
	findPortHole(char *star,char *port);
public:
// constructor: makes an empty galaxy
	InterpGalaxy() { descriptor = "An interpreted galaxy";}

// describe: same as Galaxy for now (doesn't print action list)
	virtual operator char* () { return Galaxy::operator char*(); }

// set the descriptor
	setDescriptor(char *dtext) { descriptor = dtext;}

// add a new star to the galaxy
	addStar(char *starname, char *starclass);

// add a connection
	Geodesic &
	connect(char *srcstar, char *srcport, char *dststar, char *dstport,
		int delay = 0);
// add an alias
	alias(char *galport, char *star, char *starport);

// Make a new, identical galaxy
	virtual Block *clone();

// "register" the galaxy (add it to the known list)
	addToKnownList();
			  
};
#endif
