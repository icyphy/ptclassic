static const char file_id[] = "DDFDomain.cc";
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
#include "DDFTarget.h"
#include "KnownTarget.h"
#include "DDFScheduler.h"
#include "DDFWormhole.h"
#include "AutoForkNode.h"

extern const char DDFdomainName[] = "DDF";

class DDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new DDFWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new DDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new DDFtoUniversal;}

	// new Geodesic (Node)
	Geodesic& newNode() { LOG_NEW; return *new AutoForkNode;}

	// make all galaxy as wormholes
	int isGalWorm()	{return TRUE;}

	// constructor
	DDFDomain() : Domain("DDF") {subDomains += "SDF" ;}
};

// declare a prototype
static DDFDomain proto;

// declare a prototype default target object.

static DDFTarget defaultDDFtarget;
static KnownTarget entry(defaultDDFtarget,"default-DDF");
