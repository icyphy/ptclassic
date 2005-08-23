static const char file_id[] = "C50Domain.cc";
/**********************************************************************
Version identification:
@(#)C50Domain.cc	1.3	8/15/96

@Copyright (c) 1990-1996 The Regents of the University of California.
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

 Author: Andreas Baensch
 Date of creation: 4/14/95

Modeled after similar domain kernel files by J. T. Buck.

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the C50 domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "C50Target.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "C50PortHole.h"
#include "C50Wormhole.h"
#include "AsmForkNode.h"

extern const char C50domainName[] = "C50";

class C50Domain : public Domain {
public:
	// FIXME: no fn to build a C50-on-outside wormhole yet.


	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new C50fromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new C50toUniversal;}

	// new node (geodesic)
	Geodesic& newGeo(int multi) {
		if (multi) { LOG_NEW; return *new AsmForkNode;}
		else { LOG_NEW; return *new AsmGeodesic;}
	}

	// constructor (also allow AnyAsm stars)
	C50Domain() : Domain("C50") {subDomains += "AnyAsm";}

	Star& newWorm(Galaxy& innerGal,Target* innerTarget) {
		return *new C50Wormhole(innerGal,innerTarget);
	}
	
	// require C50Target
	const char* requiredTarget(){ return "C50Target";} 
};

// declare a prototype
static C50Domain proto;

// declare the default Target object

static	C50Target defaultC50Target("default-C50", "TI 320C5x processor");

static KnownTarget entry(defaultC50Target,"default-C50");
