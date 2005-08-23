static const char file_id[] = "SDFDomain.cc";
/**********************************************************************
Version identification:
@(#)SDFDomain.cc	1.15 10/27/95

Copyright (c) 1990-1996 The Regents of the University of California.
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
 Date of creation: 7/2/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the SDF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "KnownTarget.h"
#include "SDFTarget.h"
#include "SDFWormhole.h"
#include "SDFPortHole.h"
#include "AutoForkNode.h"
#include "SDFPTclTarget.h"

extern const char SDFdomainName[] = "SDF";

class SDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new SDFWormhole(innerGal,innerTarget);
	}

	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InSDFPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutSDFPort;}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new SDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new SDFtoUniversal;}

	// constructor
  	SDFDomain() : Domain("SDF") {
          subDomains += "HOF";
        }
};

// declare a prototype
static SDFDomain proto;

// declare a prototype default target object.

static SDFTarget defaultSDFtarget("default-SDF",
"Runs SDF systems on the local workstation using either the default\n"
"one-processor SDF scheduler or Joe's clustering loop scheduler.");

static KnownTarget entryDefault(defaultSDFtarget,"default-SDF");

static SDFPTclTarget SDFPTclTargetProto("SDF-to-PTcl",
					"Generate CG PTcl Equivalent");

static KnownTarget entryPTcl(SDFPTclTargetProto,"SDF-to-PTcl");
