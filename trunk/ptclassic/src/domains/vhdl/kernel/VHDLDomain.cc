static const char file_id[] = "VHDLDomain.cc";
/**********************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmer:  Michael C. Williamson

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the VHDL domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Error.h"
#include "KnownTarget.h"
#include "VHDLForkNode.h"
#include "VHDLGeodesic.h"
#include "VHDLTarget.h"
#include "VHDLWormhole.h"

extern const char VHDLdomainName[] = "VHDL";

class VHDLDomain : public Domain {
public:
        // no XXXinVHDL wormholes yet, so no newWorm,
  	// error check in Domain.cc

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; 
		return *new VHDLfromUniversal;
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; 
		return *new VHDLtoUniversal;
	}

        // new geodesic
	Geodesic& newGeo(int multi) {
		if (multi) { LOG_NEW; return *new VHDLForkNode;}
		else { LOG_NEW; return *new VHDLGeodesic;}
	}

	// constructor
	VHDLDomain() : Domain("VHDL") {}

	// require VHDLTarget
	const char* requiredTarget() { return "VHDLTarget";}
};

// declare a prototype
static VHDLDomain proto;

// declare the default Target object

static VHDLTarget defaultVHDLtarget("default-VHDL","VHDLStar",
"Generate stand-alone VHDL programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.");

static KnownTarget entry(defaultVHDLtarget,"default-VHDL");
