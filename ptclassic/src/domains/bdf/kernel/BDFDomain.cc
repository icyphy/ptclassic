static const char file_id[] = "BDFDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck and S. Ha
 Date of creation: 8/31/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the BDF domain so the interpreter can generate them dynamically.

 Essentially a copy of DDFDomain.cc.
***********************************************************************/

#include "Domain.h"
#include "BDFTarget.h"
#include "KnownTarget.h"
#include "BDFWormhole.h"
#include "BDFConnect.h"
#include "BDFWormConnect.h"
#include "AutoForkNode.h"

extern const char BDFdomainName[] = "BDF";

class BDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		return *new BDFWormhole(innerGal,innerTarget);
	}

	// new input porthole
	PortHole& newInPort() { return *new InBDFPort;}

	// new output porthole
	PortHole& newOutPort() { return *new OutBDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { return *new BDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { return *new BDFtoUniversal;}

	// new Geodesic (Node)
	Geodesic& newNode() { return *new AutoForkNode;}

	// constructor
	BDFDomain() : Domain("BDF") {subDomains += "SDF" ;}
};

// declare a prototype
static BDFDomain proto;

// declare a prototype default target object.

static BDFTarget defaultBDFtarget;
static KnownTarget entry(defaultBDFtarget,"default-BDF");
