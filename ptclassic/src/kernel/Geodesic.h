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
Particles. Actually it stores only Particle*s, so that its
operation is independent of the type of Particle.

Geodesics can be created named or unnamed.

*****************************************************************/

#include "NamedObj.h"
#include "ParticleStack.h"

class GenericPort;
class PortHole;

	///////////////////////////////////////////
	// class Geodesic
	///////////////////////////////////////////

class Geodesic : public NamedObj {
public:
	// set the source and destination portholes -- virtual
	// these functions return a pointer to the "real porthole"
	// on success, NULL on failure.
	virtual PortHole* setSourcePort (GenericPort &, int delay = 0);
	virtual PortHole* setDestPort (GenericPort &);

	// disconnect from porthole
	virtual int disconnect (PortHole &);

	// return true if the Geodesic is persistent
	// (may exist in a disconnected state)
	virtual int isItPersistent() const;

	// return who is connected to source, destination
	PortHole* sourcePort () const { return originatingPort;}
	PortHole* destPort () const   { return destinationPort;}

        // Constructor
        Geodesic() : pstack(0)
	{ 
		originatingPort = NULL;
		destinationPort = NULL;
		numInitialParticles = sz = 0;
	}

	// Destructor -- virtual since we have subclasses
	virtual ~Geodesic();

	// initialize() for Geodesic initializes the number of Particles
	// to that given by the numInitialParticles field, and
	// also calls initialize() for each of those Particles
	// TO BE DONE:  There should be a way to specify the value
	// of these initial particles.
	virtual void initialize();

	// Put a Particle into the Geodesic
	void put(Particle* p) {pstack.putTail(p); sz++;}

	// Get a Particle from the Geodesic
	Particle* get() {
		if (sz > 0) { sz--; return pstack.get();}
		else return 0;
	}

	//  Push a Particle back into the Geodesic
	void pushBack(Particle* p) {pstack.put(p); sz++;}

	// Return the number of Particles on the Geodesic
	int size() const {return sz;}

	// Information printing
	StringList printVerbose() const;

        // A connection may require some initial particles.
        // Note that the SDFScheduler manipulates this number directly, but
        // guarantees that when it is done, the value will
        // be the same as when it started, or the run will have been aborted
        // due to a sample-rate inconsistency.
	int numInitialParticles;

protected:
	void portHoleConnect();
        PortHole *originatingPort;
        PortHole *destinationPort;
private:
	ParticleStack pstack;
	int sz;
};
#endif
