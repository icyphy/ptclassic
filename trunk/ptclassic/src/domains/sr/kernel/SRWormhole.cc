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
  Wormhole::setup();
}

void SRWormhole::go()
{

  // Move the contents of all the ports across the Event Horizon

  BlockPortIter nextPort( *((Block *) this) );
  PortHole * p;

  while ( ( p = nextPort++ ) != 0 ) {
    if ( p->isItInput() ) {
      ((SRtoUniversal *) p)->transferParticle();
    }
  }
  
  // Run the inner (foreign) galaxy

  Wormhole::run();
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

void SRtoUniversal::receiveData()
{
  getData();
  transferData();
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

// Transfer data from an outer SR domain to an inner universal event horizon
void SRtoUniversal::transferParticle()
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

    transferData();

    // cout << "The particle is now " << (*newLocation)->print() << "\n";

    // cout << "There are " << ghostAsPort()->numTokens() << " tokens on the far Geodesic\n";

    // Particle * p = ghostAsPort()->geo()->get();
    // ghostAsPort()->geo()->pushBack(p);    
    // cout << "The first particle is " << p->print() << " " << p << "\n";
  }
}
	
/**************************************************************************

	methods for SRfromUniversal

**************************************************************************/

void SRfromUniversal::sendData ()
{
	// fill in
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
