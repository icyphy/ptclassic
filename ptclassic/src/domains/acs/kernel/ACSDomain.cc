static const char file_id[] = "ACSDomain.cc";
/**********************************************************************
Copyright (c) 1990-1996 The Regents of the University of California.
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


 Programmer:  J. T. Buck
 Date of creation: 7/2/90
 Version: @(#)XDomain.ccP	1.15	07/30/96

 WARNING -- XDomain.ccP is a template file that is used to generate
 domain description modules.  If the name of this file is not XDomain.ccP,
 DO NOT EDIT IT!!!

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the ACS domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "Galaxy.h"
#include "KnownTarget.h"
#include "ACSScheduler.h"
#include "ACSWormhole.h"
#include "ACSForkNode.h"
#include "ACSGeodesic.h"
#include "ACSTarget.h"

extern const char ACSdomainName[] = "ACS";

class ACSDomain : public Domain {
public:
	// constructor
	ACSDomain() : Domain(ACSdomainName) {}

	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new ACSWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new ACSfromUniversal; }

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new ACStoUniversal; }

	// new geodesic
	Geodesic& newGeo(int multi) {
		if (multi) { LOG_NEW; return *new ACSForkNode;}
		else { LOG_NEW; return *new ACSGeodesic;}
	}

	// require ACSTarget
	const char* requiredTarget() { return "ACSTarget";}
};

// declare a prototype
static ACSDomain proto;

static ACSTarget defaultACStarget("default-CGC","ACSStar", "Runs ACS systems on the local workstation using the default\n"
"one-processor SDF scheduler.");
static KnownTarget entry(defaultACStarget, "default-ACS");

