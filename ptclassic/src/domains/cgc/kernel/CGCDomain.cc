static const char file_id[] = "CGCDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/2/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CGC domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CGCTarget.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "CGCConnect.h"
#include "CGCForkNode.h"
#include "ProcMemory.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

// For the hacks below to avoid doing wormholes:

#include "WormConnect.h"

extern const char CGCdomainName[] = "CGC";

class CGCDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		// return *new CGCWormhole(innerGal,innerTarget);
		Error::abortRun("No CGC wormhole implemented yet");
		// Following is a hack
		LOG_NEW; return *(new SDFStar);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InCGCPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutCGCPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		// return *new CGfromUniversal;
		Error::abortRun("No CGC EventHorizon implemented yet");
		// Following is a hack
		LOG_NEW; return *(new EventHorizon);
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		// return *new CGtoUniversal;
		Error::abortRun("No CGC EventHorizon implemented yet");
		// Following is a hack
		LOG_NEW; return *(new EventHorizon);
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new CGCForkNode;}

	// constructor
	CGCDomain() : Domain("CGC") {}
};

// declare a prototype
static CGCDomain proto;

// declare the default Target object

static CGCTarget defaultCGCtarget;
static KnownTarget entry(defaultCGCtarget,"default-CGC");
