static const char file_id[] = "VHDLBDomain.cc";
/**********************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
		return *new VHDLBStar;
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

	// new geodesic
	Geodesic& newGeo(int multi) {
		if (multi) { LOG_NEW; return *new VHDLBForkNode;}
		else { LOG_NEW; return *new VHDLBGeodesic;}
	}

	// constructor
	VHDLBDomain() : Domain("VHDLB") {}
};

// declare a prototype
static VHDLBDomain proto;

// declare the default Target object

static VHDLBTarget defaultVHDLBtarget("default-VHDLB","VHDLBStar",
"Generate stand-alone VHDLB programs.  The program\n"
"is written to a directory given as a Target param.\n");

static KnownTarget entry(defaultVHDLBtarget,"default-VHDLB");
