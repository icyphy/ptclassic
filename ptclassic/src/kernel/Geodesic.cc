static const char file_id[] = "Geodesic.cc";
/****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: J. T. Buck
 (original version of Geodesic was in Connect, by E. A. Lee and
  D. G. Messerschmitt -- this one is quite different)

Geodesic

Geodesic is where Particles reside in transit between
Blocks. It is a container class, which stores and retreives
Particles. Actually it stores only Particle*s, so that its
operation is independent of the type of Particle.

Geodesics can be created named or unnamed.

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Geodesic.h"
#include "Block.h"
#include "PortHole.h"
#include "Plasma.h"
#include "StringList.h"

StringList Geodesic :: print (int) const {
	StringList out;
	if (isItPersistent()) out += "Persistent ";
	out += "Geodesic: ";
	out += fullName();
	out += "\n";
	if (originatingPort) {
		out += "originatingPort: ";
		out += originatingPort->fullName();
		out += "\n";
	}
	if (destinationPort) {
		out += "destinationPort: ";
		out += destinationPort->fullName();
		out += "\n";
	}
	if (!originatingPort && !destinationPort)
		out += "Not connected.\n";
	return out;
}

int Geodesic :: isItPersistent () const {
	return FALSE;
}

// this function "wires" the connection between portholes.
// it requires friend access to class PortHole.
void Geodesic :: portHoleConnect () {
	if (originatingPort && destinationPort) {
		originatingPort->farSidePort = destinationPort;
		destinationPort->farSidePort = originatingPort;
		originatingPort->myGeodesic = this;
		destinationPort->myGeodesic = this;
	}
}

// this function changes the sourceport.
PortHole* Geodesic :: setSourcePort (GenericPort& sp, int delay) {
	originatingPort = &sp.newConnection();
	portHoleConnect();
	numInitialParticles = delay;
	return originatingPort;
}

// this function adjusts the delay.
void Geodesic :: setDelay (int delay) {
	numInitialParticles = delay;
}

PortHole* Geodesic :: setDestPort (GenericPort& dp) {
	destinationPort = &dp.newConnection();
	portHoleConnect();
	return destinationPort;
}

int Geodesic :: disconnect (PortHole& p) {
	if (originatingPort == &p) originatingPort = 0;
	else if (destinationPort == &p) destinationPort = 0;
	else return FALSE;
	return TRUE;
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	pstack.freeup();

	// Initialize the buffer to the number of Particles
	// specified in the connection; note that these Particles
	// are initialized by Plasma
	for(int i=numInitialParticles; i>0; i--) {
		Particle* p = originatingPort->myPlasma->get();
		pstack.putTail(p);
	}
	sz = numInitialParticles;
	// TO DO: Allow Particles in the Geodesic to be
	// initialized to specific values
}

// simulated run bookkeeping
void Geodesic :: incCount(int n) { sz += n;}
void Geodesic :: decCount(int n) { sz -= n;}

// destructor
Geodesic :: ~Geodesic () {
	// free all particles (return them to Plasma)
	pstack.freeup();
	// disconnect any connected portholes.  0 argument says
	// not to have PortHole::disconnect try to do things to geodesic
	if (originatingPort) originatingPort->disconnect(0);
	if (destinationPort) destinationPort->disconnect(0);
}

// isa
ISA_FUNC(Geodesic,NamedObj);
