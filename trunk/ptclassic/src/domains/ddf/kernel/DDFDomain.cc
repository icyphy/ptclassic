static const char file_id[] = "DDFDomain.cc";
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

 Programmer:  Soonhoi Ha
 Date of creation: 8/31/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the DDF domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "DDFTarget.h"
#include "KnownTarget.h"
#include "DDFScheduler.h"
#include "DDFWormhole.h"
#include "AutoForkNode.h"

extern const char DDFdomainName[] = "DDF";

class DDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new DDFWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new DDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new DDFtoUniversal;}

	// Make all galaxies wormholes.
	// This is removed for now.  It is needed for restructuring,
	// but restructuring is not working properly, and neither are
	// wormholes with arbitrary DDF properties on their boundaries.
	// int isGalWorm() {return TRUE;}

	// constructor: permit BDF and SDF as subdomains.
	DDFDomain() : Domain("DDF") {
		// note: this order selects SDFFork, not BDFFork.
		subDomains += "SDF";
		subDomains += "BDF";
	}

	// require DDFTarget
	const char* requiredTarget() { return "DDFTarget";}
};

// declare a prototype
static DDFDomain proto;

// declare a prototype default target object.

static DDFTarget defaultDDFtarget;
static KnownTarget entry(defaultDDFtarget,"default-DDF");
