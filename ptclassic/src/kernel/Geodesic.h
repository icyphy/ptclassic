#ifndef _Geodesic_h
#define _Geodesic_h 1

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
Particles. Actually it stores only Particle*'s, so that its
operation is independent of the type of Particle.
*****************************************************************/

#include "ParticleStack.h"
class PortHole;

	///////////////////////////////////////////
	// class Geodesic
	///////////////////////////////////////////

class Geodesic : private ParticleStack
{
public:
	// A lot of things are kept public, unfortunately, because
	//  these routines are called by a lot of different
	//  types of PortHoles -- impossible to make them
	//  all friends

        // We keep a pointer to the PortHoles corresponding to
        // this Geodesic
        PortHole *originatingPort;
        PortHole *destinationPort;

        // Constructor
        Geodesic() : ParticleStack(0)
	{ 
		originatingPort = NULL;
		destinationPort = NULL;
		numInitialParticles = sz = 0;
	}

	// Destructor -- frees up all particles first
	virtual ~Geodesic() {
		freeup();
	}

	// initialize() for Geodesic initializes the number of Particles
	// to that given by the numInitialParticles field, and
	// also calls initialize() for each of those Particles
	// TO BE DONE:  There should be a way to specify the value
	// of these initial particles.
	void initialize();

	// Put a Particle into the Geodesic
	void put(Particle* p) {putTail(p); sz++;}

	// Get a Particle from the Geodesic
	Particle* get();

	//  Push a Particle back into the Geodesic
	void pushBack(Particle* p) {ParticleStack::put(p); sz++;}

	// Return the number of Particles on the Geodesic
	int size() const {return sz;}

        // A connection may require some initial particles.
        // Note that the SDFScheduler manipulates this number directly, but
        // guarantees that when it is done, the value will
        // be the same as when it started, or the run will have been aborted
        // due to a sample-rate inconsistency.
	int numInitialParticles;
private:
	int sz;
};
#endif
