static const char file_id[] = "SRPortHole.cc";

/*  $Id$

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

    Author:	S. A. Edwards
    Created:	14 April 1996

    Code for the SR domain's PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRPortHole.h"
#include "SRGeodesic.h"
#include "SRStar.h"
#include "Error.h"
#include "Plasma.h"
#include <stream.h>

// Class identification.
ISA_FUNC(SRPortHole,PortHole);

// Connect this port to another
//
// @Description This is used for point-to-point connections and
// will either connect to the SRGeodesic of either port, or create a new one
// if needed.  Calls SRGeodesic::setOldDestPort and
// SRGeodesic::setOldSourcePort to make the connection because it
// calls newConnection.

void SRPortHole::connect(GenericPort& destination,
				 int,
				 const char *)
{
  SRPortHole & nearPort = (SRPortHole &) newConnection();
  SRPortHole & farPort = (SRPortHole &) destination.newConnection();

  //  cout << "connect called on "
  //       <<  nearPort.parent()->name() << " " << nearPort.name()
  //       << " to " << farPort.parent()->name() << " " << farPort.name()
  //       << "\n";
    
  enum { connect_both, connect_near, connect_far } c;

  Geodesic * g = nearPort.geo();
  if ( g ) {

    // The near port has a geodesic, so connect the far port to it

    c = connect_far;

  } else {
    g = farPort.geo();
    if ( g ) {

      // The destination has a geodesic, so connect the source to it

      nearPort.myGeodesic = g;
      c = connect_near;

    } else {

      // Nobody has a geodesic -- create one and connect both to that

      LOG_NEW; g = new SRGeodesic;
      c = connect_both;      
    }
  }

  if ( c != connect_near ) {
    farPort.myGeodesic = g;
    if ( farPort.isItInput() ) {
      ((SRGeodesic *) g)->setOldDestPort( farPort );
    } else {
      ((SRGeodesic *) g)->setSourcePort( farPort, 0, 0 );
    }
  }

  if ( c != connect_far ) {
    nearPort.myGeodesic = g;
    if ( nearPort.isItInput() ) {
      ((SRGeodesic *) g)->setOldDestPort( nearPort );
    } else {
      ((SRGeodesic *) g)->setOldSourcePort( nearPort, 0, 0 );
    }
  }

}

// Return the PortHole that drives this one
//
// @Description This goes through the geodesic to find the source port,
// rather than using the farSidePort field.

PortHole * SRPortHole::far() const
{
  // cout << "far() called on " << parent()->name() << ' ' << name() << '\n';
  if ( myGeodesic ) {
    return myGeodesic->sourcePort();
  }

  return NULL;
}

// Set the geodesic of this port
//
// @Description This also sets the farSidePort field, which is equal to
// the driver for inputs, and equal to one of the receivers (the first in the
// list) for an output.  This nonsense is for the type resolution system.

void SRPortHole::setGeodesic( Geodesic * g )
{
  myGeodesic = g;
  if ( (SRPortHole *)(far()) != this ) {
    farSidePort = far();
  } else {
    farSidePort = ((SRGeodesic *)g)->getReceiver();
  }
}

// Identify the port as an input
int InSRPort::isItInput() const
{
    return TRUE;
}

// Identify the port as an output
int OutSRPort::isItOutput() const
{
    return TRUE;
}

// Identify the port as an input
int MultiInSRPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInSRPort::newPort()
{
	LOG_NEW; SRPortHole& p = *new InSRPort;
	// cout << "Added a new port hole to " << name() << '\n';
	return installPort(p);
}

// Input/output identification.
int MultiOutSRPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutSRPort::newPort()
{
	LOG_NEW; SRPortHole& p = *new OutSRPort;
	return installPort(p);
}

// Return TRUE if the particle in the port hole is known present or absent
//
// @Description The default behavior is to signal an error--bare
// SRPortHoles should never be used.

int SRPortHole::known() const {
  Error::abortRun("known() called on class SRPortHole");
  return FALSE;
}

// Return TRUE if the particle in the port hole is (known as) absent
//
// @Description The default behavior is to signal an error--bare
// SRPortHoles should never be used.

int SRPortHole::absent() const {
  Error::abortRun("absent() called on class SRPortHole");
  return FALSE;
}

// Return TRUE if this input is (known as) present
//
// @Description The default behavior is to signal an error--bare
// SRPortHoles should never be used.

int SRPortHole::present() const {
  Error::abortRun("present() called on class SRPortHole");
  return FALSE;
}

// Return the present particle on the port
//
// @Description It is an error to call this without knowing
// SRPortHole::present() is TRUE.  The default behavior is to signal
// an error--bare SRPortHoles should never be used

Particle & SRPortHole::get() const {
  Error::abortRun("get() called on class SRPortHole");
  // A kludge-- return a particle, leaking memory!
  return *myPlasma->get();  
}

// Return TRUE if the the output is known present or absent
int OutSRPort::known() const
{
  if ( emittedParticle ) {
    return TRUE;
  }

  return FALSE;
}

// Return TRUE if the far port exists and the output is known
int InSRPort::known() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    return farPort->known();
  }

  return FALSE;
}


// Return TRUE if the particle is known absent
int OutSRPort::absent() const
{

  if ( emittedParticle == (Particle *) 1 ) {
    return TRUE;
  }

  return FALSE;
}

// Return TRUE if the port has an absent particle
//
// @Description Return TRUE if the far port exists and has an absent
// particle.
int InSRPort::absent() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    return farPort->absent();
  }

  return FALSE;
}

// Return TRUE if the particle is known present
int OutSRPort::present() const
{

  if ( emittedParticle > (Particle *) 1 ) {
    return TRUE;
  }

  return FALSE;
}

// Return the present particle on the port
//
// @Description It is an error to call this without knowing
// SRPortHole::present() is TRUE.

Particle & OutSRPort::get() const
{
  if (emittedParticle > (Particle *) 1 ) {
    return *emittedParticle;
  }
  Error::abortRun("Attempted to get an unknown or absent particle from port ",
		  name());

  // A kludge--return some particle to avoid a core dump.
  // This leaks memory!
  
  return *myPlasma->get();
}

// Return TRUE if the particle in the port hole is known present
//
// @Description Return TRUE if the far port exists and has a present
// particle.

int InSRPort::present() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    return farPort->present();
  }

  return FALSE;
}

// Return the present particle on the port
//
// @Description It is an error to call this without knowing
// SRPortHole::present() is TRUE and the port is connected to an OutSRPort.

Particle & InSRPort::get() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    return farPort->get();
  } else {
    Error::abortRun("Attempted to get a particle from unconnected port ",
		    name());
  }

  // A kludge--return some particle to avoid a core dump.
  // This leaks memory!
  return *myPlasma->get();

}

// Clear the isIndependent flag
void InSRPort::initialize()
{
  PortHole::initialize();

  isIndependent = FALSE;
}

// Make the particle absent
void OutSRPort::makeAbsent()
{
  if ( emittedParticle ) {
    Error::error("Repeated emission on port ", name() );
    if ( emittedParticle > (Particle *) 1) {
      emittedParticle->die();
    }
  }

  emittedParticle = (Particle *) 1; 
}

// Emit a particle: make it present
Particle & OutSRPort::emit()
{
  if ( emittedParticle ) {
    Error::error("Repeated emission on port ", name() );
    if ( emittedParticle > (Particle *) 1) {
      emittedParticle->die();
    }
  }

  return *(emittedParticle = myPlasma->get());
}

// Destroy the particle in the port, if any, resetting it to "unknown"
//
// @Description In general, this should only be done at the beginning
// of an instant.  Doing so at another time usually violates monotonicity.
void OutSRPort::clearPort()
{
  if ( emittedParticle > (Particle *) 1 ) {
    emittedParticle->die();
  }

  emittedParticle = (Particle *) 0;
}

// Reset the particle in the port
void OutSRPort::initialize()
{
  PortHole::initialize();

  emittedParticle = (Particle *) 0;
}

// Return a PortHole for a new connection.
//
// @Description This almost exactly replicates the behavior of
// MultiPortHole::newConnection() in the Kernel.  However, since
// the far() method is not virtual in the kernel, it incorrectly determines
// whether the port has a connection.  This casts the port to InSRPort, which
// uses the virtual far() method and gets it right.

PortHole & MultiInSRPort::newConnection() {
  // cout << "Called newConnection on " << parent()->name()
  //     << ' ' << name() << '\n';
  // first resolve aliases.
  GenericPort & real = realPort();
  // check for being aliased to a single porthole (is this possible?)
  if (!real.isItMulti()) return (PortHole&)real;
  MultiInSRPort & realmph = (MultiInSRPort &) real;
  // find an unconnected porthole in the real port
  MPHIter next(realmph);
  InSRPort * p;
  while ((p = (InSRPort *)(next++)) != 0) {
    // do the right thing if a member of realmph is an alias.
    // I think this is not really possible, but play it safe.
    InSRPort & realp = (InSRPort &) (p->newConnection());
    // realp is disconnected iff it has no far()
    if (realp.far() == NULL) return realp;
    // We cannot just check far() on p since, if
    // it is an alias, its far will be null.
    // And we don't want to return an alias anyway.
  }
  
  // no disconnected ports, make a new one.
  // NOTE: newPort() on a GalMultiPort returns the new alias porthole,
  // so we have to resolve it to its real porthole.
  return newPort().newConnection();
}

// Disconnect this PortHole, removing it from its Geodesic, if any
//
// @Disconnect This overrides PortHole::disconnect, which expects
// farSidePort to always be correct.  With the SR domain's multiple
// connections, this is impossible.
void SRPortHole::disconnect(int delGeo) {

  if ( myGeodesic ) {
    myGeodesic->disconnect(*this);
    if ( delGeo ) {
      LOG_DEL; delete myGeodesic;
    }
    myGeodesic = NULL;
  }

  farSidePort = NULL;
}

// Destructor
//
// @Description Disconnects the PortHole and attempts to
// delete its Geodesic.
SRPortHole::~SRPortHole()
{
  SRPortHole::disconnect(1);
}

