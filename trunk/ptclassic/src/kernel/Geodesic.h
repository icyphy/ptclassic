#ifndef _Geodesic_h
#define _Geodesic_h 1

#ifdef __GNUG__
#pragma interface
#endif

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

#include "NamedObj.h"
#include "ParticleStack.h"

class GenericPort;
class PortHole;

	///////////////////////////////////////////
	// class Geodesic
	///////////////////////////////////////////

class Geodesic : public NamedObj {
	friend class AutoFork;	// used to make auto-forking geodesics
public:
	// set the source and destination portholes -- virtual
	// these functions return a pointer to the "real porthole"
	// on success, NULL on failure.
	virtual PortHole* setSourcePort (GenericPort &, int delay = 0);
	virtual PortHole* setDestPort (GenericPort &);

	// disconnect from porthole
	virtual int disconnect (PortHole &);

	// change the number of delays
	virtual void setDelay (int);

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

	// class identification
	int isA(const char*) const;

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

	// return the number of initial particles
	int numInit() const {return numInitialParticles;}

	// access head and tail of queue
	Particle* head() const { return pstack.head();}
	Particle* tail() const { return pstack.tail();}

	// Information printing
	StringList print(int verbose = 0) const;

	// These methods are available for schedulers such as the
	// SDF scheduler to simulate a run and keep track of the
	// number of particles on the geodesic.
	// incCount increases the count, decCount decreases it,
	// they are virtual to allow additional bookkeeping
	// in derived classes.

	virtual void incCount(int);
	virtual void decCount(int);


protected:
	void portHoleConnect();
	// my neighbors
        PortHole *originatingPort;
        PortHole *destinationPort;

private:
        // A connection may require some initial particles.
	// This specifies the number.
	int numInitialParticles;

	// Where the particles live
	ParticleStack pstack;
	// number of particles
	int sz;
};
#endif
