static const char file_id[] = "SRWormhole.cc";

/*  Version @(#)SRWormhole.cc	1.3 4/22/96

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
    Date of creation:	18 April 1996
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRWormhole.h"
#include "SRScheduler.h"
#include "CircularBuffer.h"

// #define DEBUG_SRWORMHOLE

#ifdef DEBUG_SRWORMHOLE
#  include <stream.h>
#  include "Geodesic.h"
#endif


/*******************************************************************

	class SRWormhole methods

********************************************************************/


// Prepare for simulation
//
// @Description Identify the wormhole as reactive to the SR scheduler
// and call Wormhole::setup()

void SRWormhole::setup()
{
  // SR Wormholes are reactive--need at least one present input to fire
  reactive();

  Wormhole::setup();
}

// Produce outputs in an instant
//
// @Description Push the contents of the input ports across the Event
// Horizon into the foreign domain, run the foreign galaxy, and pull the
// output tokens back across the boundary.

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

#ifdef DEBUG_SRWORMHOLE
  cout << "Trying to call Wormhole::run()\n";
#endif

  int result;
  result = Wormhole::run();

#ifdef DEBUG_SRWORMHOLE
  cout << "Wormhole::run() returned " << result << "\n";
#endif


  // Runnable::run();

  // Move the contents of the output ports back across the Event Horizon
  // into our domain

  nextPort.reset();

  while ( ( p = nextPort++ ) != 0 ) {
    if ( p->isItOutput() ) {
      ((SRfromUniversal *) p)->sendData();
    }
  }

}

// Call wrapup on the foreign domain's target

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

// Clone the wormhole
Block * SRWormhole::clone() const
{
  LOG_NEW; return new SRWormhole(gal.clone()->asGalaxy(),
				 myTarget()->cloneTarget());
}

// Make an empty copy of the wormhole
Block * SRWormhole::makeNew() const
{
  LOG_NEW; return new SRWormhole(gal.makeNew()->asGalaxy(),
				 myTarget()->cloneTarget());
}

// Return the stop time
//
// @Description Return the value of now() from the outer scheduler.

double SRWormhole :: getStopTime()
{
  SRScheduler * sched = (SRScheduler *) outerSched();
  return sched->now();
}

/**************************************************************************

	methods for SRtoUniversal	

**************************************************************************/

// Move an incoming particle into myBuffer, possibly transfering it
// across an event horizon boundary
//
// @Description This is called in two cases:
// <UL>
// <LI> When the SRtoUniversal is OUTSIDE of a wormhole.  In which case,
//     the particle is also move across the boundary
//
// <LI> When the SRtoUniversal is INSIDE a wormhole.  Here, the outside
//     domain will move the particle (transferData() does nothing)
// </UL>

void SRtoUniversal::receiveData()
{
  if ( present() ) {

    // Put the particle on this input into myBuffer
    //  The domain on the other side of the event horizon assumee it owns any
    //  particle that comes to it through the event horizon, so we clone
    //  the particle (which the output port owns)

    Particle ** newLocation = myBuffer->next();
    *newLocation = get().clone();

#ifdef DEBUG_SRWORMHOLE
    cout << "Just transfered `" << (*newLocation)->print() << "' to universal\n";
#endif

    // Indicate that there is a new particle in myBuffer

    tokenNew = TRUE;

    // Transfer the token from my myBuffer across the EventHorizon
    // if we are OUTSIDE a wormhole

    transferData();

#ifdef DEBUG_SRWORMHOLE
    cout << "The particle is now " << (*newLocation)->print() << "\n";
    cout << "There are " << ghostAsPort()->numTokens() << " tokens on the far Geodesic\n";

    Particle * p = ghostAsPort()->geo()->get();
    ghostAsPort()->geo()->pushBack(p);    
    cout << "The first particle is " << p->print() << " " << p << "\n";
#endif
  }

}

// Initialize the port and the event horizon
void SRtoUniversal::initialize()
{
  InSRPort::initialize();
  ToEventHorizon::initialize();		
}

// Pass the isItInput query to the event horizon
int SRtoUniversal::isItInput() const
{
  return EventHorizon::isItInput();
}

// Pass the isItOutput query to the event horizon
int SRtoUniversal::isItOutput() const
{
  return EventHorizon::isItOutput();
}

// Return myself cast to an EventHorizon
EventHorizon * SRtoUniversal::asEH()
{
  return this;
}

// Return the number of firings
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
// @Description
//
// This is called in two cases:
// <UL>
// <LI> This is on the OUTSIDE of a wormhole.  In this case, transferData()
//     is called to pull the particle across the boundary and into myBuffer
//
// <LI> This is on the INSIDE of a wormhole.  In this case, the particle
//     has already been moved into the buffer and transferData() does nothing.
// </UL>

void SRfromUniversal::sendData()
{

  // If this port is on the OUTSIDE of a wormhole, transfer data across the
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

#ifdef DEBUG_SRWORMHOLE
    cout << "Just transfered `" << (*p)->print() << "' from universal\n";
#endif

    tokenNew = FALSE;

  } else {

    // No particle -- make it absent.

    makeAbsent();

#ifdef DEBUG_SRWORMHOLE
    cout << "Just made particle absent from universal\n";
#endif

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

// Return myself cast to an EventHorizon
EventHorizon * SRfromUniversal::asEH()
{
  return this;
}

// Return the number of firings
int SRfromUniversal::onlyOne() const
{
  return TRUE;
}
