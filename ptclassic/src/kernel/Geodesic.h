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

It would be most natural to multiply inherit Geodesic from
ParticleStack and NamedObj.  However, gdb 3.5 chokes, gags, and
dies on such code, and I'm willing to sacrifice some elegance
to be able to use gdb.  Hence the very NamedObj-like name, parent,
readFullName, readName members.

*****************************************************************/

#include "NamedObj.h"
#include "ParticleStack.h"

class GenericPort;
class PortHole;

	///////////////////////////////////////////
	// class Geodesic
	///////////////////////////////////////////

class Geodesic : private ParticleStack {
public:
	// set the source and destination portholes -- virtual
	// these functions return a pointer to the "real porthole"
	// on success, NULL on failure.
	virtual PortHole* setSourcePort (GenericPort &);
	virtual PortHole* setDestPort (GenericPort &);

	// disconnect from porthole
	virtual int disconnect (PortHole &);

	// return true if the Geodesic is persistent
	// (may exist in a disconnected state)
	virtual int isItPersistent();

	// return who is connected to source, destination
	PortHole* sourcePort () const { return originatingPort;}
	PortHole* destPort () const   { return destinationPort;}

        // Constructor
        Geodesic() : ParticleStack(0)
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
	void initialize();

	// Put a Particle into the Geodesic
	void put(Particle* p) {putTail(p); sz++;}

	// Get a Particle from the Geodesic
	Particle* get();

	//  Push a Particle back into the Geodesic
	void pushBack(Particle* p) {ParticleStack::put(p); sz++;}

	// Return the number of Particles on the Geodesic
	int size() const {return sz;}

	// Information printing
	StringList printVerbose();

        // A connection may require some initial particles.
        // Note that the SDFScheduler manipulates this number directly, but
        // guarantees that when it is done, the value will
        // be the same as when it started, or the run will have been aborted
        // due to a sample-rate inconsistency.
	int numInitialParticles;

	// NamedObj-like methods.  Was done by MH
	void setNameParent(const char* nm, Block* bp) {
		name = nm; blockIamIn = bp;
	}
	const char* readName() const { return name;}

	Block* parent() const { return blockIamIn;}

	StringList readFullName() const;

protected:
	void portHoleConnect();
        PortHole *originatingPort;
        PortHole *destinationPort;
private:
	int sz;
// This could also be done by multiple inheritance from NamedObj, but gdb
// chokes on that.  Once we get a new gdb I may change this back.
	const char* name;
	Block* blockIamIn;
};
#endif
