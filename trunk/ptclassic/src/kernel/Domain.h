/*******************************************************************
SCCS Version identification :
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 6/29/90

 The Domain class.  We declare one instance of each derived domain.
 This class provides a way to get the proper types of various objects
 for a domain, and allows Wormholes to be automatically generated.

*******************************************************************/

#ifndef _Domain_h
#define _Domain_h 1
class Scheduler;
class Block;
class Star;
class Galaxy;
class PortHole;
class EventHorizon;

const int NUMDOMAINS = 10;	// maximum # of domains

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
	const char* domainName() { return name;}
	static Domain* named(const char* name);
	static Domain* domainOf(Block&);
private:
	static int numDomains;
	static Domain* allDomains[NUMDOMAINS];

	int index;
	const char* name;
};
#endif
