static const char file_id[] = "StringParticle.cc";

#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
@(#)StringParticle.cc	1.3	2/29/96

Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  John Reekie
 Date of creation: 1/27/96

**************************************************************************/
#include "StringParticle.h"
// #include "StringArrayState.h"
// #include "StringMessage.h"
#include "Plasma.h"
#include "Error.h"

static StringParticle fpproto;
static Plasma stringMessagePlasma(fpproto);
static PlasmaGate stringMessageGate(stringMessagePlasma);


extern const DataType STRING = "STRING";


// The contents of an "empty" string message. This never gets initialized,
// but apparently it works in the matrix classes...
static Envelope dummy;


DataType StringParticle::type() const { return STRING;}

// Initialize a given ParticleStack with the values in the delay
// string, obtaining other Particles from the given Plasma.  
// Returns the number of total Particles initialized, including
// this one.
//
// FIXME: Code added but not tested.
//
// int StringParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
// 				   Plasma* myPlasma, const char* delay) {
//   StringArrayState initDelays;
//   initDelays.setState("initDelays",parent,delay);
//   initDelays.initialize();
//   int numInitialParticles = initDelays.size();
//   if(numInitialParticles > 0) {
//       StringMessage *str = new StringMessage(initDelays[0]);
//       Envelope p(*str);
//       data = p;
//       for(int i = 1; i < numInitialParticles; i++) {
// 	  str = new StringMessage(initDelays[i]);
// 	  Particle* p = myPlasma->get();
// 	  *p << *str;
// 	  pstack.putTail(p);
//       }
//   }
//   return numInitialParticles;
// }

// particle copy
Particle& StringParticle::operator = (const Particle& p) {
	if (compareType(p)) {
		const StringParticle& ps = *(const StringParticle*)&p;
		data = ps.data;
	}
	return *this;
}

// clone, useNew, die analogous to other particles.

Particle* StringParticle::clone() const {
	Particle * p = stringMessagePlasma.get();
	((StringParticle*)p)->data = data;
	return p;
}

Particle* StringParticle::useNew() const {
	LOG_NEW; return new StringParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other Message objects and allows them
// to be deleted when no longer required.
void StringParticle::die() {
	data = dummy;
	stringMessagePlasma.put(this);
}
