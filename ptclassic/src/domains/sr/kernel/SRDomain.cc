static const char file_id[] = "SRDomain.cc";
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

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the SR domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "Galaxy.h"
#include "Geodesic.h"
#include "KnownTarget.h"
#include "SRScheduler.h"
#include "SRStaticScheduler.h"
#include "SRRecursiveScheduler.h"
#include "SRWormhole.h"
#include "SRGeodesic.h"

extern const char SRdomainName[] = "SR";

/**********************************************************************

 The Synchronous/Reactive Domain

 @Description This domain implements a synchronous model of time
 similar to that used in the Esterel, Argos, and Lustre languages.
 Everything computes instantly, and communication takes place through
 unbuffered "wires" that take on exactly one value in an instant.  The
 value on each wire may be unknown, absent, or present with some
 value.  The semantics of the domain are the least fixed point of the
 system, and schedulers use an iterative relaxation scheme to approach
 this least fixed point.

 <P> Here are some known problems:
 <OL>
 <LI> By default, Ptolemy flattens hierarchy by always resolving
      "name aliases" (i.e., when a port on the outside of a galaxy
      is called something different on the inside).  Unfortunately,
      InterpGalaxy's nodeConnect uses Block::portWithName
      to resolve connection names, which flattens these aliases and
      nothing along the way is virtual.  The net effect of this is that
      node-based connections through InterpGalaxy don't do the right
      thing in the SR domain.

      <P> Fortunately, the point-to-point connection scheme (i.e.,
      through <TT>connect</TT> uses "newConnection" to resolve this,
      which I've overridden to not resolve aliases.
  </OL>

**********************************************************************/
class SRDomain : public Domain {
public:
  // constructor
  SRDomain() : Domain(SRdomainName) {}

  // new wormhole
  Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
    LOG_NEW; return *new SRWormhole(innerGal,innerTarget);
  }
  
  // new fromUniversal EventHorizon
  EventHorizon& newFrom() { LOG_NEW; return *new SRfromUniversal;}

  // new toUniversal EventHorizon
  EventHorizon& newTo() { LOG_NEW; return *new SRtoUniversal;}

  // Allocate a new geodesic
  // @Description This function is here to override auto-forking,
  // which is unnecessary in the SR domain
  // Geodesic & newGeo(int = FALSE) {
  //  LOG_NEW; return *(Geodesic *) new SRGeodesic;
  // }

};

// declare a prototype
static SRDomain proto;

/**********************************************************************

  The default target for the SR domain

  @Description This uses the brute-force scheduler

  @SeeAlso SRScheduler

 **********************************************************************/
class SRTarget : public Target {
public:
	// Constructor
	SRTarget() : Target("default-SR", "SRStar", "default SR target",
			    SRdomainName) {}

	// Destructor
	~SRTarget() { delSched(); }

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const { LOG_NEW; return new SRTarget;}

protected:
	void setup() {
		if (!scheduler()) { LOG_NEW; setSched(new SRScheduler); }
		Target::setup();
	}
};

static SRTarget defaultSRtarget;
static KnownTarget entry(defaultSRtarget,"default-SR");


/**********************************************************************

  The static scheduler target for the SR domain

  @Description This uses the static scheduler

 **********************************************************************/
class SRStaticTarget : public Target {
public:
	// Constructor
	SRStaticTarget() : Target("static-SR", "SRStar",
				  "SR target with static scheduling",
				  SRdomainName) {}

	// Destructor
	~SRStaticTarget() { delSched(); }

	// Return a copy of itself
	Block* makeNew() const { LOG_NEW; return new SRStaticTarget;}

protected:
	void setup() {
		if (!scheduler()) { LOG_NEW; setSched(new SRStaticScheduler); }
		Target::setup();
	}
};

static SRStaticTarget staticSRtarget;
static KnownTarget secondEntry(staticSRtarget,"static-SR");

/**********************************************************************

  The recursive scheduler target for the SR domain

  @Description This uses the recursive scheduler

 **********************************************************************/
class SRRecursiveTarget : public Target {
public:
	// Constructor
	SRRecursiveTarget() : Target("recursive-SR", "SRStar",
				  "SR target with recursive scheduling",
				  SRdomainName) {}

	// Destructor
	~SRRecursiveTarget() { delSched(); }

	// Return a copy of itself
	Block* makeNew() const { LOG_NEW; return new SRRecursiveTarget;}

protected:
	void setup() {
		if (!scheduler()) { LOG_NEW; setSched(new SRRecursiveScheduler); }
		Target::setup();
	}
};

static SRRecursiveTarget recursiveSRtarget;
static KnownTarget thirdEntry(recursiveSRtarget,"recursive-SR");
