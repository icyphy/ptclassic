static const char file_id[] = "Geodesic.cc";
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

#ifdef __GNUG__
#pragma implementation
#endif

#include "Geodesic.h"
#include "Block.h"
#include "Connect.h"
#include "Plasma.h"
#include "StringList.h"

StringList Geodesic :: printVerbose () const {
	StringList out;
	if (isItPersistent()) out += "Persistent ";
	out += "Geodesic: ";
	out += readFullName();
	out += "\n";
	if (originatingPort) {
		out += "originatingPort: ";
		out += originatingPort->readFullName();
		out += "\n";
	}
	if (destinationPort) {
		out += "destinationPort: ";
		out += destinationPort->readFullName();
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
