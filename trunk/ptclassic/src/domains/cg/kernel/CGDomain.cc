static const char file_id[] = "CGDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/2/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CG domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CGTarget.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "CGConnect.h"
#include "AutoForkNode.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

// For the hacks below to avoid doing wormholes:

#include "WormConnect.h"

extern const char CGdomainName[] = "CG";

class CGDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy&, Target*)  {
		// return *new CGWormhole(innerGal);
		Error::abortRun("No CG wormhole implemented yet");
		// Following is a hack
		LOG_NEW; return *(new SDFStar);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InCGPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutCGPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		// return *new CGfromUniversal;
		Error::abortRun("No CG EventHorizon implemented yet");
		// Following is a hack
		LOG_NEW; return *(new EventHorizon);
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		// return *new CGtoUniversal;
		Error::abortRun("No CG EventHorizon implemented yet");
		// Following is a hack
		LOG_NEW; return *(new EventHorizon);
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AutoForkNode;}

	// constructor
	CGDomain() : Domain("CG") {}
};

// declare a prototype
static CGDomain proto;

// declare the default Target object

static CGTarget defaultCGtarget("default-CG","CGStar","default CG target");
static KnownTarget entry(defaultCGtarget,"default-CG");

