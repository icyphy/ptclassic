static const char file_id[] = "CG56Domain.cc";
/**********************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
 Date of creation: 1/2/92

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the CG56 domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "CG56Target.h"
#include "KnownTarget.h"
#include "SDFScheduler.h"
#include "CG56PortHole.h"
#include "CG56Wormhole.h"
#include "AsmForkNode.h"

extern const char CG56domainName[] = "CG56"; 
class CG56Domain : public Domain {
public:
	// no fn to build a CG56-on-outside wormhole yet.

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() {
		LOG_NEW; return *new CG56fromUniversal;
	}

	// new toUniversal EventHorizon
	EventHorizon& newTo() {
		LOG_NEW; return *new CG56toUniversal;
	}

	// new node (geodesic)
	Geodesic& newNode() { LOG_NEW; return *new AsmForkNode;}

	// constructor (also allow AnyAsm stars)
	CG56Domain() : Domain("CG56") {subDomains += "AnyAsm";}
};

// declare a prototype
static CG56Domain proto;

// declare the default Target object

static CG56Target defaultCG56target("default-CG56","vanilla Motorola 56000 processor");

static KnownTarget entry(defaultCG56target,"default-CG56");
