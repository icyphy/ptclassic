static const char file_id[] = "SRPortHole.cc";

/*  Version @(#)SRPortHole.cc	1.5 5/7/96

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
#include "SRStar.h"
#include "Error.h"
#include "Plasma.h"
#include <stream.h>

// Class identification.
ISA_FUNC(SRPortHole,PortHole);

// Return myself
//
// @Description Overrides the alias-resolving version in GenericPort,
// since the SR domain doesn't want the hierarchy to be flattened.

PortHole & SRPortHole::newConnection () {
  cout << "newConnection called on " << this->parent()->name() << " "
       << this->name() << "\n";
  
  PortHole * p = (PortHole *) this;
  return *p;
}

// Connect this port to another
//
// @Description If this is an input, set the far port to the
// destination.  If this is an output, set the far port of the
// destination to point back here.  (Calls setFarPort.)

void SRPortHole::connect(GenericPort& destination,
				 int,
				 const char* = 0)
{
  // cout << "connect called on "
       // <<  this->parent()->name() << " " << this->name()
       // << " to " << destination.parent()->name() << " " << destination.name()
       // << "\n";

  SRPortHole * t = undoAliases();
  SRPortHole * d = ((SRPortHole *) (&destination))->undoAliases();  

  // cout << " (actually) "
       // <<  t->parent()->name() << " " << t->name()
       // << " to " << d->parent()->name() << " " << d->name()
       // << "\n";

  t = this;
  d = (SRPortHole *) &destination;

  if ( t->isItInput() ) {
    t->setFarPort( d );
  } else {
    d->setFarPort( t );
  }

}

SRPortHole * SRPortHole::undoAliases()
{
  SRPortHole * p = this;
  while ( p->aliasFrom() ) {
    p = (SRPortHole *) p->aliasFrom();
  }
  return p;
}

SRPortHole * SRPortHole::doAliases()
{
  return (SRPortHole *) translateAliases();
}

// Set the far port
void SRPortHole::setFarPort( SRPortHole * ) {
  // cout << "setFarPort called on non-input " << parent()->name() << " "
  //     << name() << "\n";
}

// Set the far port for this input
void InSRPort::setFarPort( SRPortHole * p ) {
  // cout << "setFarPort called on input " << parent()->name() << " "
       // << name() << "\n";
  farSidePort = (PortHole *) p;
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
	LOG_NEW; PortHole& p = *new InSRPort;
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
	LOG_NEW; PortHole& p = *new OutSRPort;
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
    Error::abortRun("Attempted to get a particle from unconnected port ", name());
  }

  // A kludge--return some particle to avoid a core dump.
  // This leaks memory!
  return *myPlasma->get();

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
