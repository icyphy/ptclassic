static const char file_id[] = "SRDomain.cc";
/**********************************************************************
Version identification:
$Id$

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

 WARNING -- XDomain.ccP is a template file that is used to generate
 domain description modules.  If the name of this file is not XDomain.ccP,
 DO NOT EDIT IT!!!

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the SR domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "KnownTarget.h"
#include "SRScheduler.h"
#include "SRWormhole.h"

extern const char SRdomainName[] = "SR";

class SRDomain : public Domain {
public:
	// new wormhole
	Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
		LOG_NEW; return *new SRWormhole(innerGal,innerTarget);
	}

	// new fromUniversal EventHorizon
	EventHorizon& newFrom() { LOG_NEW; return *new SRfromUniversal;}

	// new toUniversal EventHorizon
	EventHorizon& newTo() { LOG_NEW; return *new SRtoUniversal;}

	// constructor
	SRDomain() : Domain("SR") {}
};

// declare a prototype
static SRDomain proto;

// declare the default Target object

class SRTarget : public Target {
public:
	SRTarget() : Target("default-SR","SRStar","default SR target"){}
	Block* makeNew() const { LOG_NEW; return new SRTarget;}
	~SRTarget() { delSched();}

protected:
	void setup()
	{
		if (!scheduler()) { LOG_NEW; setSched(new SRScheduler); }
		Target::setup();
	}
};

static SRTarget defaultSRtarget;
static KnownTarget entry(defaultSRtarget,"default-SR");
