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
#include "Target.h"
#include "SDFScheduler.h"
#include "SDFWormhole.h"
#include "SDFConnect.h"
#include "SDFWormConnect.h"
#include "SDFGeodesic.h"

extern const char SDFdomainName[] = "SDF";

class SDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		return *new SDFWormhole(innerGal,innerTarget);
	}

	// new input porthole
	PortHole& newInPort() { return *new InSDFPort;}

	// new output porthole
	PortHole& newOutPort() { return *new OutSDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { return *new SDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { return *new SDFtoUniversal;}

	// new node (geodesic)
	Geodesic& newNode() { return *new SDFGeodesic;}

	// constructor
	SDFDomain() : Domain("SDF") {}
};

// declare a prototype
static SDFDomain proto;
