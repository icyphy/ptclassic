static const char file_id[] = "HOFDomain.cc";
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
 Based on Version: @(#)XDomain.ccP	1.15	7/30/96
 HOFDomains.cc Version: $Id$ G%

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the HOF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "Galaxy.h"
#include "KnownTarget.h"
#include "HOFScheduler.h"
#include "HOFWormhole.h"

extern const char HOFdomainName[] = "HOF";

class HOFDomain : public Domain {
public:
	// constructor
	HOFDomain() : Domain(HOFdomainName) {}

	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new HOFWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new HOFfromUniversal; }

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new HOFtoUniversal; }
};

// declare a prototype
static HOFDomain proto;

// declare the default Target object

class HOFTarget : public Target {
public:
	// Constructor
	HOFTarget() : Target("default-HOF", "HOFStar",
			     "default HOF target", HOFdomainName) {}

	// Destructor
	~HOFTarget() { delSched(); }

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const {
		LOG_NEW; return new HOFTarget;
	}

protected:
	void setup() {
		if (!scheduler()) { LOG_NEW; setSched(new HOFScheduler); }
		Target::setup();
	}
};

static HOFTarget defaultHOFtarget;
static KnownTarget entry(defaultHOFtarget, "default-HOF");
