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
#include "CGCPortHole.h"
#include "CGCForkNode.h"
#include "CGWormhole.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

extern const char CGCdomainName[] = "CGC";

class CGCDomain : public Domain {
public:
	// no XXXinCGC wormholes yet, so no newWorm

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; return *(new CGfromUniversal);
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; return *(new CGtoUniversal);
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new CGCForkNode;}

	// constructor
	CGCDomain() : Domain("CGC") {}
};

// declare a prototype
static CGCDomain proto;

// declare the default Target object

static CGCTarget defaultCGCtarget("default-CGC","CGCStar",
"Generate stand-alone C programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n"
"Can use either the default SDF scheduler or Joe's loop scheduler.");

static KnownTarget entry(defaultCGCtarget,"default-CGC");
