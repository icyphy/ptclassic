/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 6/29/90
 Modified: 9/22/90, by EAL, to add mechanism for reading all known
		domain names.

 The Domain class.  We declare one instance of each derived domain.
 This class provides a way to get the proper types of various objects
 for a domain, and allows Wormholes to be automatically generated.

*******************************************************************/

#ifndef _Domain_h
#define _Domain_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"

class Block;
class Star;
class Galaxy;
class PortHole;
class EventHorizon;
class Geodesic;
class Target;

const int NUMDOMAINS = 20;		// maximum # of domains

class Domain {
public:
	Domain (const char* domname) {
		index = numDomains++;
		allDomains[index] = this;
		myName = domname;
	}
	virtual Star& newWorm(Galaxy& innerGal,Target* innerTarget = 0);
	virtual EventHorizon& newFrom();
	virtual EventHorizon& newTo();
	virtual Geodesic& newNode() = 0;
	virtual int isGalWorm();
	const char* name() { return myName;}
	static Domain* named(const char* name);
	static Domain* of(Block&);
	static int number() {return numDomains;}
	static const char* nthName(int n) {return (allDomains[n])->myName;}
	StringList subDomains;
private:
	static int numDomains;
	static Domain* allDomains[NUMDOMAINS];

	int index;
	const char* myName;
};
#endif
