static const char file_id[] = "MDSDFDomain.cc";
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


 Programmer:  J. T. Buck
 Date of creation: 7/2/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the MDSDF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "KnownTarget.h"
#include "MDSDFScheduler.h"
#include "MDSDFWormhole.h"
#include "MDSDFForkNode.h"

extern const char MDSDFdomainName[] = "MDSDF";

class MDSDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new MDSDFWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new MDSDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new MDSDFtoUniversal;}

	// new geodesic
	Geodesic& newGeo(int multi) {
		if (multi) { LOG_NEW; return *new MDSDFForkNode;}
		else { LOG_NEW; return *new MDSDFGeodesic;}
	}

	// constructor
	MDSDFDomain() : Domain("MDSDF") {}
};

// declare a prototype
static MDSDFDomain proto;

// declare the default Target object

class MDSDFTarget : public Target {
public:
	MDSDFTarget() : Target("default-MDSDF","MDSDFStar","default MDSDF target"){}
	void setup()
	{
		if (!scheduler()) { LOG_NEW; setSched(new MDSDFScheduler); }
		scheduler()->setGalaxy(*galaxy());
		Target::setup();
	}
	Block* makeNew() const { LOG_NEW; return new MDSDFTarget;}
	~MDSDFTarget() { delSched();}
};

static MDSDFTarget defaultMDSDFtarget;
static KnownTarget entry(defaultMDSDFtarget,"default-MDSDF");
