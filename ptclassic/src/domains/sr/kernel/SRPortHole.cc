static const char file_id[] = "SRPortHole.cc";

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

// Class identification.
ISA_FUNC(SRPortHole,PortHole);

// Input/output identification.
int InSRPort::isItInput() const
{
    return TRUE;
}

// Input/output identification.
int OutSRPort::isItOutput() const
{
    return TRUE;
}

// Input/output identification.
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

int SRPortHole::known() const {
  Error::abortRun("known() called on class SRPortHole");
  return FALSE;
}

int SRPortHole::present() const {
  Error::abortRun("present() called on class SRPortHole");
  return FALSE;
}

// Return TRUE if the the output is known
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

// Return TRUE if the particle is known present
int OutSRPort::present() const
{

  if ( emittedParticle > (Particle *) 1 ) {
    return TRUE;
  }

  return FALSE;
}

// Return TRUE if the far port exists and has a present particle
int InSRPort::present() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    return farPort->present();
  }

  return FALSE;
}

// Return the particle that has been emitted
Particle & InSRPort::get() const
{
  OutSRPort * farPort;

  if ( (farPort = (OutSRPort *) far()) != (OutSRPort *) 0 ) {
    if ( farPort->emittedParticle > (Particle *) 1 ) {
      return *(farPort->emittedParticle);
    } else {
      Error::abortRun("Attempted to get an unknown or absent particle from port ",
		      name());
      
    }
  } else {
    Error::abortRun("Attempted to get a particle from unconnected port ", name());
  }

  // A kludge--return some particle to avoid a core dump.
  // This leaks memory!
  return *myPlasma->get();

}

// Make the particle absent
void OutSRPort::absent()
{
  if ( emittedParticle ) {
    Error::error("Repeated emission on port ", name() );
    emittedParticle->die();
  }

  emittedParticle = (Particle *) 1; 
}

// Emit a particle
Particle & OutSRPort::emit()
{
  if ( emittedParticle ) {
    Error::error("Repeated emission on port ", name() );
    emittedParticle->die();
  }

  return *(emittedParticle = myPlasma->get());  
}

// Destroy the particle in the port, if any, resetting it to "unknown"
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
