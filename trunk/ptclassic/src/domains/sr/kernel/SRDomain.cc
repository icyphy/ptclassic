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

 Version: $Id$

 A device to produce the correct portholes, wormholes, event horizons,
 etc, for the SR domain so the interpreter can generate them dynamically.

***********************************************************************/

#include "Domain.h"
#include "Target.h"
#include "Galaxy.h"
#include "SRGeodesic.h"
#include "KnownTarget.h"
#include "StringState.h"
#include "SRScheduler.h"
#include "SRRecursiveScheduler.h"
#include "SRWormhole.h"
#include "SRGeodesic.h"
#include "SRParter.h"

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
 this least fixed point. <P>

 This class is responsible for returning SR domain wormholes, event horizons,
 and geodesics.

**********************************************************************/
class SRDomain : public Domain {
public:
  SRDomain() : Domain(SRdomainName) {}

  // Return a newly-created wormhole
  Star& newWorm(Galaxy& innerGal,Target* innerTarget)  {
    LOG_NEW; return *new SRWormhole(innerGal,innerTarget);
  }
  
  // Return a new fromUniversal Event Horizon
  EventHorizon& newFrom() { LOG_NEW; return *new SRfromUniversal;}

  // Return a new toUniversal Event Horizon
  EventHorizon& newTo() { LOG_NEW; return *new SRtoUniversal;}

  // Return a new geodesic
  Geodesic & newGeo(int = FALSE) {
    LOG_NEW; return *(Geodesic *) new SRGeodesic;
  }

};

// declare a prototype
static SRDomain proto;

/**********************************************************************

  The dynamic target for the SR domain

  @Description This uses the brute-force dynamic scheduler.

  @SeeAlso SRScheduler

 **********************************************************************/
class SRDynamicTarget : public Target {
public:
  SRDynamicTarget()
    : Target("dynamic-SR", "SRStar", "SR target with dynamic scheduling",
		      SRdomainName) {}

  ~SRDynamicTarget() { delSched(); }

  // Return a new SRTarget
  Block * makeNew() const { LOG_NEW; return new SRDynamicTarget;}

protected:
  // Create a new scheduler (if necessary) and call Target::setup()
  void setup() {
    if (!scheduler()) { LOG_NEW; setSched(new SRScheduler); }
    Target::setup();
  }

};

static SRDynamicTarget dynamicSRtarget;
static KnownTarget dynamicEntry(dynamicSRtarget,"dynamic-SR");

/**********************************************************************

  The default target for the SR domain

  @Description This uses the recursive scheduler.

  <P> It has a parameter "partScheme" which selects which partitioning
  scheme (one of the SRPart-derived classes) to use.

 **********************************************************************/
class SRRecursiveTarget : public Target {
public:
  SRRecursiveTarget() : Target("default-SR", "SRStar",
			       "SR target with recursive scheduling",
			       SRdomainName) {
    addState( partScheme.setState( "partScheme", this, "sweep",
				   "Partitioning scheme:\n"
				   "sweep : Greedy kernel sweep\n"
				   "one : Single-vertex partitions only\n"
				   "exact : Exact -- consider all partitions\n"
				   "inout : Greedy minimum indegree/outdegree\n" ));
  };

  ~SRRecursiveTarget() { delSched(); }

  // Return a new SRRecursiveTarget
  Block * makeNew() const { LOG_NEW; return new SRRecursiveTarget;}

protected:

  // Create a new scheduler (if necessary) and call Target::setup()
  //
  // @Description Also call setParter to set up the selected
  // partition scheme.
  void setup() {
    if (!scheduler()) { LOG_NEW; setSched(new SRRecursiveScheduler); }

    // Select the partitioning scheme
    SRParter::setParter(partScheme);

    Target::setup();
  }

  // Selects which partitioning scheme to use
  StringState partScheme;
};

static SRRecursiveTarget recursiveSRtarget;
static KnownTarget recursiveEntry(recursiveSRtarget,"default-SR");


