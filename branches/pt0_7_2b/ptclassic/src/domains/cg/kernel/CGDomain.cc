/**********************************************************************
Version identification:
@(#)XDomain.ccP	1.3	11/8/90  (CGDomain.cc)

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/2/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CG domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CGScheduler.h"
// #include "CGWormhole.h"
#include "CGConnect.h"
// #include "CGWormConnect.h"
#include "CGGeodesic.h"
// for error messages (temporary, until wormholes exist):
#include "Output.h"

// For the hacks below to avoid doing wormholes:
#include "SDFStar.h"
#include "WormConnect.h"

extern const char CGdomainName[] = "CG";

class CGDomain : public Domain {
public:
	// new scheduler
	Scheduler& newSched() { return *new CGScheduler;}

	// new wormhole
	Star& newWorm(Galaxy& innerGal)  {
		// return *new CGWormhole(innerGal);
		Error::abortRun("No CG wormhole implemented yet");
		// Following is a hack
		return *(new SDFStar);
	}

	// new input porthole
	PortHole& newInPort() { return *new InCGPort;}

	// new output porthole
	PortHole& newOutPort() { return *new OutCGPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		// return *new CGfromUniversal;
		Error::abortRun("No CG EventHorizon implemented yet");
		// Following is a hack
		return *(new EventHorizon);
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		// return *new CGtoUniversal;
		Error::abortRun("No CG EventHorizon implemented yet");
		// Following is a hack
		return *(new EventHorizon);
	}

	// new node (geodesic)
	Geodesic& newNode() { return *new CGGeodesic;}

	// constructor
	CGDomain() : Domain("CG") {}
};

// declare a prototype
static CGDomain proto;
