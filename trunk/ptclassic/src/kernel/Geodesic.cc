/****************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

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

#include "Geodesic.h"
#include "Block.h"
#include "Connect.h"
#include "Plasma.h"
#include "StringList.h"

int Geodesic :: isItPersistent () {
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

Particle* Geodesic::get()
{
	if (sz > 0) {
		sz--;
		return ParticleStack::get();
	}
	else return 0;
}

void Geodesic :: initialize()
{
	// Remove any Particles residing on the Geodesic,
	// and put them in Plasma
	freeup();

	// Initialize the buffer to the number of Particles
	// specified in the connection; note that these Particles
	// are initialized by Plasma
	for(int i=numInitialParticles; i>0; i--) {
		Particle* p = originatingPort->myPlasma->get();
		putTail(p);
	}
	sz = numInitialParticles;
	// TO DO: Allow Particles in the Geodesic to be
	// initialized to specific values
}

// destructor
Geodesic :: ~Geodesic () {
	// free all particles (return them to Plasma)
	freeup();
	// disconnect any connected portholes.  0 argument says
	// not to have PortHole::disconnect try to do things to geodesic
	if (originatingPort) originatingPort->disconnect(0);
	if (destinationPort) destinationPort->disconnect(0);
}


// readFullName
StringList Geodesic :: readFullName () const {
	StringList out;
	if (blockIamIn) {
		out = blockIamIn->readFullName();
		out += ".";
	}
	out += name;
	return out;
}
