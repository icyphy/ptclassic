static const char file_id[] = "DEDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck, Soonhoi Ha
 Date of creation: 8/18/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the DE domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "DETarget.h"
#include "KnownTarget.h"
#include "DEScheduler.h"
#include "DEWormhole.h"
#include "DEPortHole.h"
#include "AutoForkNode.h"

extern const char DEdomainName[] = "DE";

class DEDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new DEWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new DEfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new DEtoUniversal;}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AutoForkNode;}

	// constructor
	DEDomain() : Domain("DE") {}
};

// declare a prototype
static DEDomain proto;

static DETarget defaultDEtarget;
static KnownTarget entry(defaultDEtarget,"default-DE");
