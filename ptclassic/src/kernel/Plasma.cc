static const char file_id[] = "Plasma.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1993 The Regents of the University of California.
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

 Programmer: J. Buck  
 Date of creation: 6/23/93 (formerly part of Particle.cc)

ParticleStack and Plasma methods.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#pragma implementation "ParticleStack.h"
#endif

#include "Plasma.h"
#include "Error.h"

// ParticleStack methods
// Destructor -- deletes all Particles EXCEPT the last one
// we don't delete the last one because it's the "reference"
// particle (the first one); it normally isn't a dynamically
// created object.

ParticleStack :: ~ParticleStack () {
	if (!pHead) return;
	Particle* p;
	while (pHead->link) {
		p = pHead;
		pHead = pHead->link;
		LOG_DEL; delete p;
	}
}

// freeup -- returns all Particles to their Plasma (including
// the last one)
void ParticleStack :: freeup () {
	Particle* p;
	while (pHead) {
		p = pHead;
		pHead = pHead->link;
		p->die();
	}
}

// class Plasma

// constructor for class Plasma.  p is the root particle.
// correspondingGlobalP is 0 by default, which means that the plasma
// is global; if a value is given, we are making a local plasma and
// it points to the corresponding global Plasma.

Plasma :: Plasma(Particle& p,Plasma* correspondingGlobal)
: ParticleStack(&p), gate(0), refCount(0)
{
	if (correspondingGlobal) {
		// I am a local plasma, and globalP is the corresponding
		// global Plasma
		nextPlasma = correspondingGlobal;
		localFlag = TRUE;
	}
	else {
		nextPlasma = plasmaList;
		plasmaList = this;
		localFlag = FALSE;
	}
}

// general action for Plasma::get().
Particle* Plasma :: slowGet() {
	CriticalSection region(gate);
	if (moreThanOne()) return ParticleStack::get();
	else if (isLocal()) return nextPlasma->get();
	else return head()->useNew();
}

void Plasma :: slowPut(Particle* p) {
	CriticalSection region(gate);
	ParticleStack::put(&p->initialize());
}

Plasma* Plasma :: getPlasma(DataType t)
{
	Plasma* p = plasmaList;

	while (p) {
		DataType dt = p->type();
		if (t == dt) return p;
		p = p->nextPlasma;
	}
	if(t == ANYTYPE)
		Error::abortRun("can't create Plasma with type ANYTYPE");
	else {
		StringList msg = "unknown Particle type: ";
		msg += t;
		Error::abortRun(msg);
	}
	return 0;
}

// create a new local Plasma of the given type and return a pointer to it.
// 0 is returned if no such type is found.
Plasma* Plasma :: makeNew(DataType t)
{
	Plasma* gp = getPlasma(t);
	if (!gp) return 0;
	LOG_NEW; return new Plasma(*(gp->tail()),gp);
}

// locking functions for Plasmas.
void Plasma :: makeLock(const PtGate& master) {
	if (!isLocal()) return;
	LOG_DEL; delete gate;
	gate = master.clone();
}

void Plasma :: delLock() {
	if (!isLocal()) return;
	LOG_DEL; delete gate;
	gate = 0;
}

