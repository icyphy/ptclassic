static const char file_id[] = "$RCSfile$";

/*  Version $Id$

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	6 April 1992

*/

#include "Domain.h"
#include "KnownTarget.h"
#include "MTDFTarget.h"
#include "MTDFScheduler.h"
#include "MTDFWormhole.h"
#include "MTDFConnect.h"
#include "MTDFWormConnect.h"
#include "MTDFGeodesic.h"


extern const char MTDFdomainName[] = "MTDF";

class MTDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new MTDFWormhole(innerGal,innerTarget);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InMTDFPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutMTDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new MTDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new MTDFtoUniversal;}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new MTDFGeodesic;}

	// constructor
	MTDFDomain() : Domain(MTDFdomainName) {}
};

// declare a prototype
static MTDFDomain protoDomain;

// Add a prototype MTDFTarget to the KnownTarget list.
static MTDFTarget protoTarget;
static KnownTarget entry(protoTarget,"default-MTDF");
