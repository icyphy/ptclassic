#ifndef _Plasma_h
#define _Plasma_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 10/31/90
 Revisions:
	The original version of Plasma was by D. Messerschmitt and
	E. A. Lee.  This version has been modified considerably; it
	is derived from ParticleStack, which uses members of the
	Particles themselves to form a stack.

A Plasma is a collection of currently unused Particles

Particles no longer needed are added to the Plasma; any
OutPortHoles needing a Particle gets it from the Plasma

There is precisely one instance of Plasma for each
Particle type needed -- this common pool makes more
efficient use of memory.  The static member plasmaList
is a list of all Plasmas.  Each Plasma is a node on the
linked list plasmaList.

***************************************************************/

#include "ParticleStack.h"

	////////////////////////////////////////
	// class Plasma
	////////////////////////////////////////

class Plasma : public ParticleStack {
public:
	// Put a Particle into the Plasma
	// (inherited, uses ParticleStack::put)

	// Get a Particle from the Plasma, creating a
	// new one if necessary.  Never give away last Particle
	Particle* get() { 
		if (moreThanOne()) {
			Particle* p = ParticleStack::get();
			p->initialize();
			return p;
		}
		else return head->useNew();
	}
	DataType type() { return head->readType();}

// constructor -- all objs built are added to the static linked list.
	Plasma(Particle& p) : ParticleStack(&p), nextPlasma(plasmaList) {
		plasmaList = this;
	}

// get the appropriate Plasma object given a type
	static Plasma* getPlasma (DataType t);
private:
// The global list of Plasmas (points to head of list)
	static Plasma* plasmaList;
// pointer to next Plasma on the list
	Plasma* nextPlasma;

};

#endif
