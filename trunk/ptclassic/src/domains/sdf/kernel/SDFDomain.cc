static const char file_id[] = "SDFDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/2/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the SDF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "KnownTarget.h"
#include "SDFTarget.h"
#include "SDFWormhole.h"
#include "SDFPortHole.h"
#include "AutoForkNode.h"

extern const char SDFdomainName[] = "SDF";

class SDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new SDFWormhole(innerGal,innerTarget);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InSDFPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutSDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new SDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new SDFtoUniversal;}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AutoForkNode;}

	// constructor
	SDFDomain() : Domain("SDF") {}
};

// declare a prototype
static SDFDomain proto;

// declare a prototype default target object.

static SDFTarget defaultSDFtarget;
static KnownTarget entry(defaultSDFtarget,"default-SDF");
