/* -*- C++ -*- */

#ifndef _SRWormhole_h
#define _SRWormhole_h

/*  Version @(#)SRWormhole.h	1.3 4/22/96

Copyright (c) 1990- The Regents of the University of California.
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

    Programmer:		S. A. Edwards
    Date of creation:	18 April 1992
*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Wormhole.h"
#include "SRStar.h"
#include "EventHorizon.h"
#include "SRPortHole.h"

/**********************************************************************

  The wormhole for the SR domain

  @Description This wormhole makes other universes appear as strict SR
  stars (i.e., all of their inputs must be known before the go()
  method is called).  Moreover, go() is called exactly once in an instant. <P>

  Present values send tokens, absent or unknown values send nothing.
  Arriving tokens make a present value.  The absence of an arriving token
  makes an absent value.  <P>

  Defining a wormhole amounts to defining three classes:

  <OL>

  <LI> SRWormhole, an SRStar that contains a foreign domain

  <LI> SRtoUniversal, an InSRPort that attaches to SRWormholes and
       moves information into foreign domains--a ToEventHorizon

  <LI> SRfromUniversal, an OutSRPort that attaches to SRWormholes and
       moves information from foreign domains---a FromEventHorizon

  </OL>

A few confusing things:

  <P> isItInput and isItOutput indicate the direction of the porthole
  <EM>on the outside of the wormhole</EM>.  Thus, both ports in a
  toUniversal and fromUniversal indicate the same direction, even
  though one is a "to" and the other is a "from."  This wierdness is
  used by transferData() to determine when it is responsible for
  moving particles.

  <P> EventHorizon::moveFromGhost is mis-named--it should be moveToGhost, as
  it moves particles FROM its own buffer TO the port given as an argument.

  <P> FromEventHorizon::transferData() doesn't always--in one case, it's being
  called from inside a wormhole and does not.

<HR>

<P> There are four cases for information flow:

<PRE>
    Outside domain: SR        Inside Domain: XXX
			  |--SRWormhole-----------
			  |
1.  ---->   SRtoUniversal | XXXfromUniversal ---->     (isItInput = TRUE)
			  |
			  |-----------------------
</PRE>

   <P> It's the responsibility of SRtoUniversal to "push" particles across
   the boundary to the XXXfromUniversal port.  To do this,

   <OL>

 <LI> Place the particle in the SRtoUniversal's myBuffer
      CircularBuffer.  The XXXfromUniversal will own this particle
      (i.e., call die() on it).  Since the OutSRPorts in the SR domain
      nominally own the particles, a clone is made.

 <LI> Set the SRtoUniversal::tokenNew flag to TRUE, indicating a new
      particle is ready to be transfered.

 <LI> Call ToEventHorizon::transferData().  This moves the particle
      from the SRtoUniversal's myBuffer across the event horizon.

      <P> If the tokenNew flag is TRUE, transferData() calls moveFromGhost
      to move the particle from the SRtoUniversal's myBuffer to the
      XXXfromUniversal's myBuffer.  It then calls the
      XXXfromUniversal's sendData(), which moves the particles "into
      the domain" from the buffer.  E.g., in the SDF domain, moves a
      particle from the XXXfromUniversal's buffer to the attached
      Geodesic.

  </OL>

   These must happen before the SRWormhole fires, but exactly when
   depends on the domain.  In the SR domain, these are done inside
   SRWormhole::go() just before Wormhole::run() is called.

<HR>

<PRE>
    Outside domain: SR        Inside Domain: XXX
			  |--SRWormhole-----------
			  |
2.  <---- SRfromUniversal | XXXtoUniversal   <----     (isItOutput = TRUE)
			  |
			  |-----------------------
</PRE>

   <P> The SRfromUniversal must "pull" particles across the boundary from
   the XXXtoUniversal port.  To do this,

 <OL>
 <LI> Call FromEventHorizon::transferData() on the SRfromUniversal.
      If there is a particle to transfer, this moves an outgoing
      particle from the XXXtoUniversal's myBuffer to the
      SRfromUniversal's myBuffer and sets the
      SRfromUniversal::newToken flag.

 <LI> Remove the particle from SRfromUniversal::myBuffer and send it
      on its way.

 </OL>

<HR>

<PRE>
    Outside domain: XXX       Inside Domain: SR
			   |--SRWormhole-----------
			   |
3.  ---->   XXXtoUniversal | SRfromUniversal ---->     (isItInput = TRUE)
			   |
			   |-----------------------
</PRE>

   <P> The XXXtoUniversal object will "push" particles into the
   SRfromUniversal.  The new particle, if any, will be moved into the
   SRfromUniversal::myBuffer, and SRfromUniversal::sendData() will be
   called.  To make this work:

 <OL>

 <LI> Make SRfromUniversal::sendData() move a particle from its
      myBuffer into the domain.

 </OL>

<HR>

<PRE>
    Outside domain: XXX       Inside Domain: SR
			   |--SRWormhole-----------
			   |
4.  <---- XXXfromUniversal | SRtoUniversal   <----
			   |
 			   |-----------------------
</PRE>

    <P> The XXXfromUniversal will "pull" particles from the SRtoUniversal
    by calling FromEventHorizon::tranferData(), which calls
    SRtoUniversal::receiveData().  To make this work:

  <OL>

  <LI> Make SRtoUniversal::receiveData() move the incoming particle,
       if any, into the SRtoUniversal::myBuffer.  The event horizon
       will own this particle, so cloning is necessary in the SR case.

  </OL>

 **********************************************************************/
class SRWormhole : public Wormhole, public SRStar {
public:

  SRWormhole(Galaxy& g, Target* t=0);

  ~SRWormhole();

  // Call Wormhole::begin
  void begin() { Wormhole::begin(); }
  void wrapup();

  // Return my target's scheduler
  Scheduler * scheduler() const { return myTarget()->scheduler(); }

  Block * clone() const;
  Block * makeNew() const;

  // Identify myself as a wormhole
  int isItWormhole() const { return TRUE; }

  // Return states from the foreign galaxy
  State * stateWithName (const char* name) {
    return gal.stateWithName(name);
  }

  // Initialize the foreign galaxy's states 
  void initState() { gal.initState(); }

  // Return information about the wormhole
  StringList print(int verbose) const {
    return Wormhole::print(verbose);
  }

protected:
  void setup();
  void go();

  double getStopTime();
};

/**********************************************************************

  Conversion from data in the SR domain to the Universal Event Horizon

  @Description This can be on the inside or outside of a wormhole. <P>

  Only present particles are transferred.  When the SR channel is absent
  or unknown, no particle is transferred.

**********************************************************************/
class SRtoUniversal : public ToEventHorizon, public InSRPort {
public:

  // Constructor
  SRtoUniversal() : ToEventHorizon(this) {}

  void receiveData();

  void initialize();

  int isItInput() const;
  int isItOutput() const;

  EventHorizon* asEH();

  // Return a new Geodesic
  Geodesic* allocateGeodesic() {
    return ToEventHorizon::allocateGeodesic();
  }

  int onlyOne() const;

};

/**********************************************************************

  Conversion from data in a Universal Event Horizon to the SR domain

  @Description This can be on the inside or outside of a wormhole. <P>

  When a token arrives, the port has that present particle.  When no
  token has arrived, the port has absent.

**********************************************************************/
class SRfromUniversal : public FromEventHorizon, public OutSRPort {
public:

  SRfromUniversal() : FromEventHorizon(this) {}

  void sendData();

  void transferParticle();

  void initialize();

  int isItInput() const;
  int isItOutput() const;

  EventHorizon* asEH();

  // Return a new Geodesic
  Geodesic* allocateGeodesic() {
    return FromEventHorizon::allocateGeodesic();
  }

  int onlyOne() const;
};

#endif
