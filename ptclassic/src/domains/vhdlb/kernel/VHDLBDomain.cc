static const char file_id[] = "VHDLBDomain.cc";
/**********************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/2/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the VHDLB domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "KnownTarget.h"
#include "VHDLBTarget.h"
#include "VHDLBPortHole.h"
#include "VHDLBForkNode.h"
#include "VHDLBWormhole.h"

// for error messages (temporary, until wormholes exist):
#include "Error.h"

#include "EventHorizon.h"

extern const char VHDLBdomainName[] = "VHDLB";

class VHDLBDomain : public Domain {
public:
/*
	// new wormhole
	Star& newWorm(Galaxy& ,Target* )  {
		// return *new VHDLBWormhole(innerGal,innerTarget);
		// Error::abortRun("No VHDLB wormhole implemented yet");
		// Following is a hack
		LOG_NEW;
		return *(new VHDLBStar);
	}
*/
	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InVHDLBPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutVHDLBPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; 
		return *new VHDLBFromUniversal;
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; 
		return *new VHDLBToUniversal;
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new VHDLBForkNode;}

	// constructor
	VHDLBDomain() : Domain("VHDLB") {}
};

// declare a prototype
static VHDLBDomain proto;

// declare the default Target object

static VHDLBTarget defaultVHDLBtarget("default-VHDLB","VHDLBStar",
"Generate stand-alone VHDLB programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n");

static KnownTarget entry(defaultVHDLBtarget,"default-VHDLB");
