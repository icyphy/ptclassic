static const char file_id[] = "BDFDomain.cc";
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

 Programmer:  J. Buck and S. Ha
 Date of creation: 8/31/90

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the BDF domain so the interpreter can generate them dynamically.

 Essentially a copy of DDFDomain.cc.
***********************************************************************/

#include "Domain.h"
#include "BDFTarget.h"
#include "KnownTarget.h"
#include "BDFWormhole.h"
#include "AutoForkNode.h"

extern const char BDFdomainName[] = "BDF";

class BDFDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		return *new BDFWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { return *new BDFfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { return *new BDFtoUniversal;}

	// constructor
	BDFDomain() : Domain("BDF") {subDomains += "SDF" ;}
};

// declare a prototype
static BDFDomain proto;

// declare a prototype default target object.

static BDFTarget defaultBDFtarget;
static KnownTarget entry(defaultBDFtarget,"default-BDF");

