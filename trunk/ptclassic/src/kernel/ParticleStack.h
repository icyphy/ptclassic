#ifndef _ParticleStack_h
#define _ParticleStack_h
/***************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/31/90
 Revisions:

A ParticleStack is a stack (or queue, since we can insert from
both ends) of Particles.  The links for the stack are in the
Particles themselves, so there is no memory allocation overhead
for pushing and popping.

The get method does NOT check for an empty ParticleStack; users
of the class must use the empty() method to check this.

ParticleStack is used as the baseclass in implementing Plasma
and Geodesic.
***************************************************************/

#include "Particle.h"

	////////////////////////////////////////
	// class ParticleStack
	////////////////////////////////////////

class ParticleStack {
public:
	// put a particle onto the stack
	void put(Particle* p) {
		if (!head) tail = p;
		p->link = head; head = p;
	}

	// get a particle from the stack.
	// derived classes must do the empty-stack check!
	// if we get the last, tail will still point to it.
	Particle* get() {
		Particle* p = head;
		head = p->link;
		return p;
	}

	// put an article on the "tail" of the stack (to use the
	// stack like a queue)
	void putTail(Particle* p) {
		if (head) tail->link = p;
		else head = p;	// first particle
		tail = p;	// point to new last particle
		p->link = 0;	// terminate the chain
	}

	// empty stack check
	int empty() const { return head ? 0 : 1;}

	// return true if more than one member (for Plasma)
	int moreThanOne() const { return (head && head->link) ? 1 : 0;}

	// constructor (built with 0 or one particle)
	ParticleStack(Particle*h) : head(h), tail(h) {
		// make sure the stack is "terminated"
		if (h) h->link = 0;
	}

	// destructor -- delete all particles
	~ParticleStack ();

	// return all Particles to their Plasmas
	void freeup ();
protected:
	Particle* head;
private:
	Particle* tail;
};
#endif
