/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 8/31/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the DDF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "DDFScheduler.h"
#include "DDFWormhole.h"
#include "DDFConnect.h"
#include "DDFWormConnect.h"
#include "AutoForkNode.h"

extern const char DDFdomainName[] = "DDF";

class DDFDomain : public Domain {
public:
	// new scheduler
	Scheduler& newSched() { return *new DDFScheduler;}

	// new wormhole
	Star& newWorm(Galaxy& innerGal)  {
		return *new DDFWormhole(innerGal);
	}

	// new input porthole
	PortHole& newInPort() { return *new InDDFPort;}

	// new output porthole
	PortHole& newOutPort() { return *new OutDDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { return *new DDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { return *new DDFtoUniversal;}

	// new Geodesic (Node)
	Geodesic& newNode() { return *new AutoForkNode;}

	// constructor
	DDFDomain() : Domain("DDF") {subDomains += "SDF" ;}
};

// declare a prototype
static DDFDomain proto;
