static const char file_id[] = "SRWormhole.cc";

/*  Version $Id$

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

    Programmer:		S. A. Edwards
    Date of creation:	18 April 1996
*/

/***********************************************************************

			WORMHOLE BEHAVIOR

Defining a wormhole amounts to defining three classes:

SRWormhole, an SRStar that contains a foreign domain

SRtoUniversal, an InSRPort that attaches to SRWormholes and moves information
	into foreign domains--a ToEventHorizon

SRfromUniversal, an OutSRPort that attaches to SRWormholes and moves
	information from foreign domains---a FromEventHorizon

A few confusing things:

  isItInput and isItOutput indicate the direction of the porthole ON THE
  OUTSIDE OF THE WORMHOLE.  Thus, both ports in a toUniversal and
  fromUniversal indicate the same direction, even though one is a "to"
  and the other is a "from."  This wierdness is used by transferData()
  to determine when it is responsible for moving particles.

  EventHorizon::moveFromGhost is mis-named--it should be moveToGhost, as
  it moves particles FROM its own buffer TO the port given as an argument.

  FromEventHorizon::transferData() doesn't always--in one case, it's being
  called from inside a wormhole and does not.

There are four cases for information flow:

    Outside domain: SR        Inside Domain: XXX
			  |--SRWormhole-----------
			  |
1.  ---->   SRtoUniversal | XXXfromUniversal ---->     (isItInput = TRUE)
			  |
			  |-----------------------

   It's the responsibility of SRtoUniversal to "push" particles across
   the boundary to the XXXfromUniversal port.  To do this,

   1. Place the particle in the SRtoUniversal's myBuffer
      CircularBuffer.  The XXXfromUniversal will own this particle
      (i.e., call die() on it).  Since the OutSRPorts in the SR domain
      nominally own the particles, a clone is made.

   2. Set the SRtoUniversal::tokenNew flag to TRUE, indicating a new
      particle is ready to be transfered.

   3. Call ToEventHorizon::transferData().  This moves the particle
      from the SRtoUniversal's myBuffer across the event horizon.

      If the tokenNew flag is TRUE, transferData() calls moveFromGhost
      to move the particle from the SRtoUniversal's myBuffer to the
      XXXfromUniversal's myBuffer.  It then calls the
      XXXfromUniversal's sendData(), which moves the particles "into
      the domain" from the buffer.  E.g., in the SDF domain, moves a
      particle from the XXXfromUniversal's buffer to the attached
      Geodesic.

   These must happen before the SRWormhole fires, but exactly when
   depends on the domain.  In the SR domain, these are done inside
   SRWormhole::go() just before Wormhole::run() is called.

   ----------------------------------------------------------------------

    Outside domain: SR        Inside Domain: XXX
			  |--SRWormhole-----------
			  |
2.  <---- SRfromUniversal | XXXtoUniversal   <----     (isItOutput = TRUE)
			  |
			  |-----------------------

   The SRfromUniversal must "pull" particles across the boundary from
   the XXXtoUniversal port.  To do this,

   1. Call FromEventHorizon::transferData() on the SRfromUniversal.
      If there is a particle to transfer, this moves an outgoing
      particle from the XXXtoUniversal's myBuffer to the
      SRfromUniversal's myBuffer and sets the
      SRfromUniversal::newToken flag.

   2. Remove the particle from SRfromUniversal::myBuffer and send it
      on its way.

   ----------------------------------------------------------------------

    Outside domain: XXX       Inside Domain: SR
			   |--SRWormhole-----------
			   |
3.  ---->   XXXtoUniversal | SRfromUniversal ---->     (isItInput = TRUE)
			   |
			   |-----------------------

   The XXXtoUniversal object will "push" particles into the
   SRfromUniversal.  The new particle, if any, will be moved into the
   SRfromUniversal::myBuffer, and SRfromUniversal::sendData() will be
   called.  To make this work:

   1. Make SRfromUniversal::sendData() move a particle from its
      myBuffer into the domain.

   ----------------------------------------------------------------------

    Outside domain: XXX       Inside Domain: SR
			   |--SRWormhole-----------
			   |
4.  <---- XXXfromUniversal | SRtoUniversal   <----
			   |
 			   |-----------------------

    The XXXfromUniversal will "pull" particles from the SRtoUniversal
    by calling FromEventHorizon::tranferData(), which calls
    SRtoUniversal::receiveData().  To make this work:

    1. Make SRtoUniversal::receiveData() move the incoming particle,
       if any, into the SRtoUniversal::myBuffer.  The event horizon
       will own this particle, so cloning is necessary in the SR case.

************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRWormhole.h"
#include "SRScheduler.h"
#include "CircularBuffer.h"

// #include <stream.h>
// #include "Geodesic.h"



/*******************************************************************

	class SRWormhole methods

********************************************************************/

void SRWormhole::setup()
{
  // SR Wormholes are reactive--need at least one present input to fire
  reactive();

  Wormhole::setup();
}

void SRWormhole::go()
{

  // Move the contents of the input ports across the Event Horizon into
  // the foreign domain

  BlockPortIter nextPort( *((Block *) this) );
  PortHole * p;

  while ( ( p = nextPort++ ) != 0 ) {
    if ( p->isItInput() ) {
      ((SRtoUniversal *) p)->receiveData();
    }
  }
  
  // Run the inner (foreign) galaxy

  Wormhole::run();

  // Move the contents of the output ports back across the Event Horizon
  // into our domain

  nextPort.reset();

  while ( ( p = nextPort++ ) != 0 ) {
    if ( p->isItOutput() ) {
      ((SRfromUniversal *) p)->sendData();
    }
  }

}

void SRWormhole::wrapup()
{
  myTarget()->wrapup();
}

// Constructor
SRWormhole::SRWormhole(Galaxy& g,Target* t)
  : Wormhole(*this,g,t)
{
  buildEventHorizons();
}

// Destructor
SRWormhole::~SRWormhole()
{
  freeContents();
}

// cloner -- clone the inside and make a new wormhole from that.
Block * SRWormhole::clone() const
{
  LOG_NEW; return new SRWormhole(gal.clone()->asGalaxy(),
				 myTarget()->cloneTarget());
}

Block * SRWormhole::makeNew() const
{
  LOG_NEW; return new SRWormhole(gal.makeNew()->asGalaxy(),
				 myTarget()->cloneTarget());
}

// return stop time
double SRWormhole :: getStopTime()
{
  SRScheduler * sched = (SRScheduler *) outerSched();
  return sched->now();
}

/**************************************************************************

	methods for SRtoUniversal	

**************************************************************************/

// Move an incoming particle into myBuffer, possibly transfering it
// across an event horizon boundary:
//
// This is called in two cases:
//
//   * When the SRtoUniversal is OUTSIDE of a wormhole.  In which case,
//     the particle is also move across the boundary
//
//   * When the SRtoUniversal is INSIDE a wormhole.  Here, the outside
//     domain will move the particle (transferData() does nothing)
//
void SRtoUniversal::receiveData()
{
  if ( present() ) {

    // Put the particle on this input into myBuffer
    //  The domain on the other side of the event horizon assumee it owns any
    //  particle that comes to it through the event horizon, so we clone
    //  the particle (which the output port owns)

    Particle ** newLocation = myBuffer->next();
    *newLocation = get().clone();

    // cout << "Just transfered " << (*newLocation)->print() << "\n";
    
    // Indicate that there is a new particle in myBuffer

    tokenNew = TRUE;

    // Transfer the token from my myBuffer across the EventHorizon
    // if we are OUTSIDE a wormhole

    transferData();

    // cout << "The particle is now " << (*newLocation)->print() << "\n";

    // cout << "There are " << ghostAsPort()->numTokens() << " tokens on the far Geodesic\n";

    // Particle * p = ghostAsPort()->geo()->get();
    // ghostAsPort()->geo()->pushBack(p);    
    // cout << "The first particle is " << p->print() << " " << p << "\n";
  }

}

void SRtoUniversal::initialize()
{
  InSRPort::initialize();
  ToEventHorizon::initialize();		
}

int SRtoUniversal::isItInput() const
{
  return EventHorizon::isItInput();
}

int SRtoUniversal::isItOutput() const
{
  return EventHorizon::isItOutput();
}

EventHorizon * SRtoUniversal::asEH()
{
  return this;
}

int SRtoUniversal::onlyOne() const
{
  return TRUE;
}
	
/**************************************************************************

	methods for SRfromUniversal

**************************************************************************/

// Move a particle from myBuffer into the output port, if one has been placed
// there by the wormhole
//
// This is called in two cases:
//
// * This is on the OUTSIDE of a wormhole.  In this case, transferData()
//   is called to pull the particle across the boundary and into myBuffer
//
// * This is on the INSIDE of a wormhole.  In this case, the particle
//   has already been moved into the buffer and transferData() does nothing.

void SRfromUniversal::sendData()
{

  // If this port is on the OUTS1IDE of a wormhole, transfer data across the
  // wormhole boundary (otherwise do nothing)

  transferData();

  if ( tokenNew ) {

    // Get the particle from the buffer and emit it

    Particle ** p = myBuffer->next();

    // When this port is on the INSIDE of a wormhole, it will never
    // be cleared, since it's not attached to a star in the SR domain,
    // so clear it out before emitting something.

    clearPort();

    emit() = **p;

    tokenNew = FALSE;
  }

}

void SRfromUniversal::initialize()
{
	OutSRPort::initialize();
	FromEventHorizon::initialize();
}

int SRfromUniversal::isItInput() const
{
  return EventHorizon::isItInput();
}

int SRfromUniversal::isItOutput() const
{
  return EventHorizon::isItOutput();
}

EventHorizon * SRfromUniversal::asEH()
{
  return this;
}

int SRfromUniversal::onlyOne() const
{
  return TRUE;
}
