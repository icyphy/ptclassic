static const char file_id[] = "CG56Domain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/2/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CG56 domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "CG56Connect.h"
#include "CG56WormConnect.h"
#include "AsmForkNode.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

// For the hacks below to avoid doing wormholes:

#include "WormConnect.h"

extern const char CG56domainName[] = "CG56";

class CG56Domain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		// return *new CG56Wormhole(innerGal,innerTarget);
		Error::abortRun("No CG56 wormhole implemented yet");
		// Following is a hack
		LOG_NEW; return *(new SDFStar);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InCG56Port;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutCG56Port;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; return *new CG56fromUniversal;
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; return *new CG56toUniversal;
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AsmForkNode;}

	// constructor
	CG56Domain() : Domain("CG56") {}
};

// declare a prototype
static CG56Domain proto;

// declare the default Target object

static CG56Target defaultCG56target("default-CG56","vanilla Motorola 56000 processor");

static KnownTarget entry(defaultCG56target,"default-CG56");
