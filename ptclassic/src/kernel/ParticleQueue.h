#ifndef _ParticleQueue_h
#define _ParticleQueue_h

#ifdef __GNUG__
#pragma interface
#endif
/***************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/22/91
 Revisions:

A ParticleQueue maintains a queue of particles with a possibly finite
capacity.  It is built around the ParticleStack class.

Operations: getq inserts a particle into the queue, putq gets a
particle from the queue and copies its data into the user-supplied
particle.  setCapacity sets the capacity.

getq fails (returning FALSE) if there is no data in the queue;
putq fails (returning FALSE) if the queue is full.

empty and full are predicates returning true if the queue is empty/full.
length returns the queue length, capacity returns the capacity.
initialize discards the particles in the queue and optionally sets
the capacity.

***************************************************************/
#include "ParticleStack.h"

const unsigned int QSIZE_UNLIM = ~0;

class ParticleQueue {
public:
	// constructor: starts empty, unlimited capacity by default
	ParticleQueue() : stk(0), count(0),
			  maxCount(QSIZE_UNLIM) {}

	// constructor: specified capacity
	ParticleQueue(unsigned int cap) : stk(0), count(0), maxCount(cap) {}

	// destructor: free all Particles
	~ParticleQueue() {
		stk.freeup();
	}

	// status inquiry functions
	int empty() const { return stk.empty();}
	int full() const { return count >= maxCount;}
	unsigned int capacity() const { return maxCount;}
	unsigned int length() const { return count;}

	// put a particle into the queue: fails if capacity exceeded
	int putq(Particle& p);

	// get a particle from the queue, copy into user-supplied particle
	// fails if queue empty
	int getq(Particle& p);

	// modify the capacity
	void setCapacity(int sz);

	// free up the queue contents
	void initialize() { stk.freeup();}

	// free up and set capacity (good for star start() methods)
	void initialize(int n) { initialize(); setCapacity(n);}
protected:
	unsigned int count;
	unsigned int maxCount;
	ParticleStack stk;
};

#endif
