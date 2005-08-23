static const char file_id[] = "CGDomain.cc";
/**********************************************************************
Version identification:
@(#)CGDomain.cc	1.14	05/21/96

Copyright (c) 1990-1997 The Regents of the University of California.
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
 etc, for the CG domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CGTarget.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "CGPortHole.h"
#include "AsmForkNode.h"
#include "CGGeodesic.h"
#include "CGWormhole.h"
#include "AsmTarget.h"

// For the hacks below to avoid doing wormholes:

extern const char CGdomainName[] = "CG";

class CGDomain : public Domain {
public:
	// new input porthole
	PortHole& newInPort() { LOG_NEW; return *new InCGPort;}

	// new output porthole
	PortHole& newOutPort() { LOG_NEW; return *new OutCGPort;}

	// new fromUniversal EventHorizon
        EventHorizon& newFrom() {
                LOG_NEW; return *new CGfromUniversal;
        }

	// new toUniversal EventHorizon
        EventHorizon& newTo() {
                LOG_NEW; return *new CGtoUniversal;
        }

	// new geodesic
	Geodesic& newGeo(int multi) {
	        // FIXME - this will work but is a hack.  We should 
	        // define CGForkNode.  Since no CG domain is derived from
	        // this class it should be ok.
		if (multi) { LOG_NEW; return *new AsmForkNode;}
		else { LOG_NEW; return *new CGGeodesic;}
	}

	// constructor
	CGDomain() : Domain("CG") {}

	// require CGTarget
	const char* requiredTarget() { return "CGTarget";}
};

// declare a prototype

static CGDomain proto;

// declare the default Target objects
// Purify will complain that the dynamic memory allocated in the
// constructor is not destroyed by the program, which is true
// because the instances are static.  Memory is freed on program exit.

static CGTarget defaultCGtarget("default-CG", "CGStar", "default CG target");
static KnownTarget entryDefault(defaultCGtarget, "default-CG");

static AsmTarget defaultAsmTarget("default-Asm", "default Asm target",
				  "CGStar");
static KnownTarget entryAsm(defaultAsmTarget, "default-Asm");

