static const char file_id[] = "ACSDomain.cc";
/**********************************************************************
Copyright (c) 1990-%Q% The Regents of the University of California.
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
 Version: $Id$

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
#include "ACSTarget.h"

extern const char ACSdomainName[] = "ACS";

class ACSDomain : public Domain {
public:
	// constructor
	ACSDomain() : Domain(ACSdomainName) {subDomains += "AnyCG";}

	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new ACSWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new ACSfromUniversal; }

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new ACStoUniversal; }
};

// declare a prototype
static ACSDomain proto;

// declare the default Target object

//class ACSTarget : public Target {
//public:
	// Constructor
//	ACSTarget() : Target("default-ACS", "ACSStar",
//			     "default ACS target", ACSdomainName) {}

	// Destructor
//	~ACSTarget() { delSched(); }

	// Return a copy of itself
//	/*virtual*/ Block* makeNew() const {
//		LOG_NEW; return new ACSTarget;
//	}

//protected:
//	void setup() {
//		if (!scheduler()) { LOG_NEW; setSched(new ACSScheduler); }
//		Target::setup();
//	}
//};

static ACSTarget defaultACStarget("default-ACS", "ACSStar",
"Runs ACS systems on the local workstation using the default\n"
"one-processor SDF scheduler.");
static KnownTarget entry(defaultACStarget, "default-ACS");
