#ifndef _Plasma_h
#define _Plasma_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
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

There is precisely one global instance of Plasma for each
Particle type needed -- this common pool makes more
efficient use of memory.  The static member plasmaList
is a list of all global Plasmas.  Each global Plasma is a
node on the linked list plasmaList.

There may also be any number of local Plasmas of each type, which
permit separate pools to be used for a given subsystem.  When a
local Plasma runs out of particles, it obtains them from the
corresponding global Plasma.  The advantage of local Plasmas
is for multithreading: if both ends of the local Plasma are in
the same thread, no lock is needed.

***************************************************************/

#include "ParticleStack.h"
#include "PtGate.h"

	////////////////////////////////////////
	// class Plasma
	////////////////////////////////////////

class Plasma : public ParticleStack {
    friend class PlasmaGate;
public:
	// constructor.  By default, we make global Plasmas.  If globalP
	// is non-null, we are making a local Plasma and globalP is the
	// corresponding global Plasma.
	Plasma(Particle& p,Plasma* globalP = 0);

	// Put a Particle into the Plasma
	void put(Particle* p) {
		if (gate == 0)
			ParticleStack::put(&(p->initialize()));
		else slowPut(p);
	}

	// Get a Particle from the Plasma, creating a
	// new one if necessary.  We do it inline for the fastest
	// case.
	Particle* get() {
		return (moreThanOne() && gate == 0) ? ParticleStack::get()
			: slowGet();
	}

	// type of the Plasma
	DataType type() const { return head()->type();}

	// is Plasma local?
	int isLocal() const { return localFlag;}

	// get the appropriate global Plasma object given a type
	static Plasma* getPlasma (DataType t);

	// create a local Plasma object given a type
	static Plasma* makeNew (DataType t);

	// create a lock for the Plasma, because it crosses
	// thead boundaries.  A do-nothing for global plasmas.
	void makeLock(const PtGate& master);

	// delete lock for the Plasma.  No effect on global plasmas.
	void delLock();

	// inc reference count, when adding reference from PortHole to
	// a local Plasma.  New count is returned.  Global Plasmas
	// pretend their count is always 1.
	short incCount() { return localFlag ? ++refCount : 1;}

	// dec reference count, when removing reference from PortHole to
	// a local Plasma.  New count is returned.  Idea is we can
	// delete it if it drops to zero.  Global Plasmas pretend
	// their count is always 1.
	short decCount() { return localFlag ? --refCount : 1;}
	
protected:
	// general implementation of "get"
	virtual Particle* slowGet();
	// same for "put"
	virtual void slowPut(Particle*);

private:
// The global list of Plasmas (points to head of chain of Plasmas)
	static Plasma* plasmaList;
// pointer to next Plasma on the list
	Plasma* nextPlasma;
// gate for locking
	PtGate* gate;
// flag, true if local
	short localFlag;
// reference count for local Plasmas.
	short refCount;
};

// a type of GateKeeper that can read Plasma's gate.
class PlasmaGate : public GateKeeper {
public:
	PlasmaGate(Plasma& plas) : GateKeeper(plas.gate) {}
};

#endif
