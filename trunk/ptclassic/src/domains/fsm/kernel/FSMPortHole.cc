static const char file_id[] = "FSMPortHole.cc";

/*  $Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

    Programmer:		Bilung Lee
    Date of creation:	3/3/96

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "FSMPortHole.h"
#include "Geodesic.h"
#include "CircularBuffer.h"

// Class identification.
ISA_FUNC(FSMPortHole,PortHole);

void InFSMPort :: receiveData () {
  if (!myGeodesic->size()) {
    // If no particle in geodesic, nothing to receive.
    dataNew = FALSE;
  } else {

    getParticle(); 
    dataNew = TRUE;
  }
}

Particle& InFSMPort :: get() {
  // Get the current particle in the buffer.
  Particle* p = *(myBuffer->here());

  if (myGeodesic->size()) {
    // If there is still particle in geodesic, 
    // get the next particle into porthole.
    // FIXME: after getParticle(), p will be put into plasma.
    getParticle(); 
    dataNew = TRUE;

  } else {
    dataNew = FALSE;
  }

  return *p;
}

void InFSMPort :: sendData () {
  // Clear unused particles in myGeodesic.
  myGeodesic->initialize();
}

// Input/output identification.
int InFSMPort::isItInput() const { return TRUE; }

void OutFSMPort :: receiveData () { clearParticle(); }

Particle& OutFSMPort :: put()
{
  // If there is a new Particle, send it to Geodesic.
  if (dataNew) sendData();

  // Return a current Particle 
  dataNew = TRUE;
  return **(myBuffer->here());
}	

void OutFSMPort :: sendData () {
  // If data in porthole is not new, then nothing to be send out to geodesic.
  if (!dataNew) return;

  putParticle(); 
  dataNew = FALSE;
}

// Input/output identification.
int OutFSMPort::isItOutput() const { return TRUE; }

// Input/output identification.
int MultiInFSMPort::isItInput() const { return TRUE; }

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInFSMPort::newPort()
{
	LOG_NEW; PortHole& p = *new InFSMPort;
	return installPort(p);
}

// Input/output identification.
int MultiOutFSMPort::isItOutput() const { return TRUE; }

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutFSMPort::newPort()
{
	LOG_NEW; PortHole& p = *new OutFSMPort;
	return installPort(p);
}
