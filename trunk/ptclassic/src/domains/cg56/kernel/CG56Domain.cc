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
#include "CG56PortHole.h"
#include "SDFWormhole.h"
#include "AsmForkNode.h"

extern const char CG56domainName[] = "CG56";

class CG56Domain : public Domain {
public:
	// no fn to build a CG56-on-outside wormhole yet.

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; return *new SDFfromUniversal;
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; return *new SDFtoUniversal;
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AsmForkNode;}

	// constructor (also allow AnyAsm stars)
	CG56Domain() : Domain("CG56") {subDomains += "AnyAsm";}
};

// declare a prototype
static CG56Domain proto;

// declare the default Target object

static CG56Target defaultCG56target("default-CG56","vanilla Motorola 56000 processor");

static KnownTarget entry(defaultCG56target,"default-CG56");
