static const char file_id[] = "StringParticle.cc";

#ifdef __GNUG__
#pragma implementation
#endif

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
#include "StringArrayState.h"
#include "StringMessage.h"
#include "Plasma.h"
#include "Error.h"

static StringParticle fpproto;
static Plasma stringMessagePlasma(fpproto);
static PlasmaGate stringMessageGate(stringMessagePlasma);


extern const DataType STRING = "STRING";


// The contents of an "empty" string message. This never gets initialized,
// but apparently it works in the matrix classes...
static Envelope dummy;


StringParticle::StringParticle(const Envelope& p) : data(p) {}

StringParticle::StringParticle() {}

DataType StringParticle::type() const { return STRING;}

StringParticle::operator int () const {
	return data.asInt();
}

StringParticle::operator double () const {
	return data.asFloat();
}

StringParticle::operator float () const {
	return float(data.asFloat());
}

StringParticle::operator Complex () const {
	return data.asComplex();
}

StringParticle::operator Fix () const {
	return data.asFix();
}


StringList StringParticle::print() const {
	return data.print();
}

// get the Message from the StringParticle.
void StringParticle::accessMessage (Envelope& p) const {
	p = data;
}

// get the Message and remove the reference (by setting data to dummy)
void StringParticle::getMessage (Envelope& p) {
	p = data;
	data = dummy;
}

Particle& StringParticle::initialize() { data = dummy; return *this;}

// Initialize a given ParticleStack with the values in the delay
// string, obtaining other Particles from the given Plasma.  
// Returns the number of total Particles initialized, including
// this one.  This should be redefined by the specific message class.
// 3/2/94 added
int StringParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
				   Plasma* myPlasma, const char* delay) {
  StringArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  int numInitialParticles = initDelays.size();
  if(numInitialParticles > 0) {
      StringMessage *str = new StringMessage(initDelays[0]);
      Envelope p(*str);
      data = p;
      for(int i = 1; i < numInitialParticles; i++) {
	  str = new StringMessage(initDelays[i]);
	  Particle* p = myPlasma->get();
	  *p << *str;
	  pstack.putTail(p);
      }
  }
  return numInitialParticles;
}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().
// FIXME: These should be defined.  No reason not to.

void StringParticle::operator << (int) { errorAssign("int");}
void StringParticle::operator << (double) { errorAssign("double");}
void StringParticle::operator << (const Complex&) { errorAssign("complex");}
void StringParticle::operator << (const Fix&) { errorAssign("fix");}

// only loader that works.
void StringParticle::operator << (const Envelope& p) { data = p;}

// particle copy
Particle& StringParticle::operator = (const Particle& p) {
	if (compareType(p)) {
		const StringParticle& ps = *(const StringParticle*)&p;
		data = ps.data;
	}
	return *this;
}

// particle compare: considered equal if Message addresses
// are the same.
// FIXME: This should compare string values.
int StringParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Envelope pkt;
	p.accessMessage(pkt);
	return data.myData() == pkt.myData();
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

// FIXME: Remove this when the assignements above are defined.
void StringParticle::errorAssign(const char* argType) const {
	StringList msg = "Attempt to assign type ";
	msg += argType;
	msg += " to a StringParticle";
	Error::abortRun(msg);
}

