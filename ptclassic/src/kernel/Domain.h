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
#pragma once
#pragma interface
#endif

#include "StringList.h"

class Scheduler;
class Block;
class Star;
class Galaxy;
class PortHole;
class EventHorizon;
class Geodesic;

const int NUMDOMAINS = 10;		// maximum # of domains

class Domain {
public:
	Domain (const char* domname) {
		index = numDomains++;
		allDomains[index] = this;
		name = domname;
	}
	virtual Scheduler& newSched() = 0;
	virtual Star& newWorm(Galaxy& innerGal) = 0;
	virtual PortHole& newInPort() = 0;
	virtual PortHole& newOutPort() = 0;
	virtual EventHorizon& newFrom() = 0;
	virtual EventHorizon& newTo() = 0;
	virtual Geodesic& newNode() = 0;
	virtual int isGalWorm() {return FALSE;}
	const char* domainName() { return name;}
	static Domain* named(const char* name);
	static Domain* domainOf(Block&);
	static int numberOfDomains() {return numDomains;}
	static const char* nthDomainName(int n) {return (allDomains[n])->name;}
	StringList subDomains;
private:
	static int numDomains;
	static Domain* allDomains[NUMDOMAINS];

	int index;
	const char* name;
};
#endif
