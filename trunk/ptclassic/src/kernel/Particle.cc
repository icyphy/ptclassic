static const char file_id[] = "Particle.cc";
/**************************************************************************
Version identification:
@(#)Particle.cc	2.33 10/02/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer:  E. A. Lee, J. Buck, and D. G. Messerschmitt
 Date of creation: 1/17/90
 Revisions:

 Definition of some methods in Particle.h

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Particle.h"
#include "Plasma.h"
#include "Error.h"
#include "ComplexArrayState.h"   // 3/2/94 added for initParticleStack
#include "FixArrayState.h"       // 3/2/94 added for initParticleStack
#include "FloatArrayState.h"     // 3/2/94 added for initParticleStack
#include "IntArrayState.h"       // 3/2/94 added for initParticleStack
#include <math.h>

// Here are the plasmas!
// Create instances of each particle and plasma

static IntParticle iproto;
static FloatParticle fproto;
static ComplexParticle cproto;
static FixParticle xproto;

Plasma* Plasma :: plasmaList = 0;
static Plasma intPlasma(iproto);
static Plasma floatPlasma(fproto);
static Plasma complexPlasma(cproto);
static Plasma fixPlasma(xproto);

// Create locks for the global Plasmas.
static PlasmaGate intGate(intPlasma);
static PlasmaGate floatGate(floatPlasma);
static PlasmaGate complexGate(complexPlasma);
static PlasmaGate fixGate(fixPlasma);

	///////////////////////////////////////
	// class Particle
	///////////////////////////////////////

// return true if types are equal; return false and print error if not.
int Particle :: compareType (const Particle& src) const {
	if (typesEqual(src)) return 1;
	StringList msg = "Attempt to copy ";
	msg += src.type();
	msg += " Particle to ";
	msg += type();
	msg += " Particle";
	Error::abortRun (msg);
	return 0;
}

extern const DataType ANYTYPE = "ANYTYPE";

#include "SubMatrix.h"

Particle::operator ComplexSubMatrix* () const { return NULL; }  //added
Particle::operator FixSubMatrix* () const { return NULL; }      //added
Particle::operator FloatSubMatrix* () const { return NULL; }    //added
Particle::operator IntSubMatrix* () const { return NULL; }      //added

	///////////////////////////////////////
	// class IntParticle
	///////////////////////////////////////

extern const DataType INT = "INT";

Particle* IntParticle :: clone () const {
	Particle* p = intPlasma.get();
	((IntParticle*)p)->data = data;
	return p;
}

void IntParticle :: die () { intPlasma.put(this);}

Particle* IntParticle :: useNew () const { LOG_NEW; return new IntParticle;}

Particle& IntParticle :: operator = (const Particle& p)
{
	data = int(p);
	return *this;
}

int IntParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == int(p);
}

DataType IntParticle :: type() const {return INT;}
IntParticle :: operator int () const {return data;}
IntParticle :: operator double () const {return double(data);}
IntParticle :: operator float () const {return float(data);}
IntParticle :: operator Complex () const {return Complex(data);}
IntParticle :: operator Fix () const {return Fix(double(data));}

StringList IntParticle :: print () const { return StringList(data);}

	// Wash the Particle
Particle& IntParticle :: initialize() {data=0; return *this;}

        // Initialize a given ParticleStack with the values in delay,
        // obtaining other particles from the given plasma.  Returns
        // the number of total particles initialized, including this one.
        // 3/2/94 added 
int IntParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
				   Plasma* myPlasma, const char* delay) {
  IntArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  int numInitialParticles = initDelays.size();
  if(numInitialParticles > 0) {
    data = initDelays[0];
    for(int i = 1; i < numInitialParticles; i++) {
      Particle* p = myPlasma->get();
      *p << initDelays[i];
      pstack.putTail(p);
    }
  }
  return numInitialParticles;
}

	// Load up with data
void IntParticle :: operator << (int i) {data=i;}
void IntParticle :: operator << (double f) {data=int(floor(f + 0.5));}
void IntParticle :: operator << (const Complex& c) {data=int(floor(abs(c) + 0.5));}
void IntParticle :: operator << (const Fix& x) {data = int(x);}
void IntParticle :: operator << (const Envelope&) {
	Error::abortRun ("Attempt to load a Message into Int Particle");
}


	////////////////////////////////////////
	// class FloatParticle
	////////////////////////////////////////

extern const DataType FLOAT = "FLOAT";

Particle* FloatParticle :: useNew () const
{ LOG_NEW; return new FloatParticle;}

Particle* FloatParticle :: clone () const { 
	Particle* p = floatPlasma.get();
	((FloatParticle*)p)->data = data;
	return p;
}


void FloatParticle :: die () { floatPlasma.put(this);}

Particle& FloatParticle :: operator = (const Particle& p) {
	data = double(p);
  	return *this;
}

int FloatParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == double(p);
}



DataType FloatParticle :: type() const {return FLOAT;}

        // Cast to an int, double, and Complex
FloatParticle :: operator int () const {return int(floor(data + 0.5));}
FloatParticle :: operator double () const {return data;}
FloatParticle :: operator float () const {return float(data);}
FloatParticle :: operator Complex () const {return Complex(data);}
FloatParticle :: operator Fix () const {return Fix(data);}

StringList FloatParticle :: print () const { return StringList(data);}

 
        // Initialize the Particle
Particle& FloatParticle :: initialize() {data=0.0; return *this;}

        // Initialize a given ParticleStack with the values in delay,
        // obtaining other particles from the given plasma.  Returns
        // the number of total particles initialized, including this one.
        // 3/2/94 added 
int FloatParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
				     Plasma* myPlasma, const char* delay) {
  FloatArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  int numInitialParticles = initDelays.size();
  if(numInitialParticles > 0) {
    data = initDelays[0];
    for(int i = 1; i < numInitialParticles; i++) {
      Particle* p = myPlasma->get();
      *p << initDelays[i];
      pstack.putTail(p);
    }
  }
  return numInitialParticles;
}

        // Load up with data
void FloatParticle :: operator << (int i) {data=i;}
void FloatParticle :: operator << (double f) {data=f;}
void FloatParticle :: operator << (const Complex& c) {data=abs(c);}
void FloatParticle :: operator << (const Fix& x) {data = double(x);}
void FloatParticle :: operator << (const Envelope&) {
	Error::abortRun ("Attempt to load a Message into Float Particle");
}


        ////////////////////////////////////////
        // class ComplexParticle
        ////////////////////////////////////////

extern const DataType COMPLEX = "COMPLEX";

ComplexParticle :: ComplexParticle(double f) {data= f;}
ComplexParticle :: ComplexParticle(int i) {data = double(i);}
ComplexParticle :: ComplexParticle() {data=0.0;}

Particle* ComplexParticle :: useNew () const
{ LOG_NEW; return new ComplexParticle;}

Particle* ComplexParticle :: clone () const {
	Particle* p = complexPlasma.get();
	((ComplexParticle*)p)->data = data;
	return p;
}

void ComplexParticle :: die () { complexPlasma.put(this);}

Particle& ComplexParticle :: operator = (const Particle& p) {
        // We use a temporary variable to avoid gcc2.7.2/2.8 problems
        Complex t = p;
        data = t;
        //data = Complex((const Complex&)p);
        return *this;
}

int ComplexParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == Complex((const Complex&)p);
}

DataType ComplexParticle :: type() const {return COMPLEX;}
 
        // Cast to an int, double, Complex
	// when casting to a real, we use the magnitude

ComplexParticle :: operator int () const {return int(floor(abs(data) + 0.5));}
ComplexParticle :: operator double () const {return abs(data);}
ComplexParticle :: operator float () const {return float(abs(data));}
ComplexParticle :: operator Complex () const {return data;}
ComplexParticle :: operator Fix () const {return Fix(abs(data));}

StringList ComplexParticle :: print () const {
	StringList out = "(";
	out += data.real();
	out += ",";
	out += data.imag();
	out += ")";
	return out;
}
 
        // Initialize the Particle
Particle& ComplexParticle :: initialize() {data=0.0; return *this;}

        // Initialize a given ParticleStack with the values in delay,
        // obtaining other particles from the given plasma.  Returns
        // the number of total particles initialized, including this one.
        // 3/2/94 added 
int ComplexParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
				       Plasma* myPlasma, const char* delay) {
  ComplexArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  int numInitialParticles = initDelays.size();
  if(numInitialParticles > 0) {
    data = initDelays[0];
    for(int i = 1; i < numInitialParticles; i++) {
      Particle* p = myPlasma->get();
      *p << initDelays[i];
      pstack.putTail(p);
    }
  }
  return numInitialParticles;
}
        // Load up with data
void ComplexParticle :: operator << (int i) {data=Complex(i);}
void ComplexParticle :: operator << (double f) {data=Complex(f);}
void ComplexParticle :: operator << (const Complex& c) {data=c;}
void ComplexParticle :: operator << (const Fix& x) 
                                              {data = Complex(double(x));}
void ComplexParticle :: operator << (const Envelope&) {
	Error::abortRun ("Attempt to load a Message into Complex Particle");
}


        ///////////////////////////////////////
        // class FixParticle
        ///////////////////////////////////////

extern const DataType FIX = "FIX";

FixParticle :: FixParticle() {}
FixParticle :: FixParticle(int len, int intBits) { data = Fix(len, intBits);}
FixParticle :: FixParticle(double& d) { data = d;}
FixParticle :: FixParticle(int len, int intBits, double& d)
                                      { data = Fix(len, intBits, d); }
FixParticle :: FixParticle(const FixParticle& x) { data = x.data; }
FixParticle :: FixParticle(int len, int intBits, FixParticle& x)
                                      { data = Fix(len, intBits, x.data); }

Particle* FixParticle :: clone () const {
        Particle* p = fixPlasma.get();
        ((FixParticle*)p)->data = data;
        return p;
}

void FixParticle :: die () { fixPlasma.put(this);}

Particle* FixParticle :: useNew () const { LOG_NEW; return new FixParticle;}

Particle& FixParticle :: operator = (const Particle& p) {
        // We use a temporary variable to avoid gcc2.7.2/2.8 problems
        Fix t = p;
        data = t;
        //data = Fix((const Fix &)p);
        return *this;
}

int FixParticle :: operator == (const Particle& p) {
         if (!typesEqual(p)) return 0;
         Fix pdata = Fix((const Fix &)p);
         return data == pdata;
}

DataType FixParticle :: type() const {return FIX;}
FixParticle :: operator int () const {return int(data);}
FixParticle :: operator double () const {return double(data);}
FixParticle :: operator float () const {return float(data);}
FixParticle :: operator Complex () const {return Complex(double(data));}
FixParticle :: operator Fix () const {return data;}

StringList FixParticle :: print () const
{       double d = double(data);
        return StringList(d);
}

        // Wash the Particle
Particle& FixParticle :: initialize() {data.initialize(); return *this;}

        // Initialize a given ParticleStack with the values in delay,
        // obtaining other particles from the given plasma.  Returns
        // the number of total particles initialized, including this one.
        // 3/2/94 added 
int FixParticle::initParticleStack(Block* parent, ParticleStack& pstack, 
				   Plasma* myPlasma, const char* delay) {
  FixArrayState initDelays;
  initDelays.setState("initDelays",parent,delay);
  initDelays.initialize();
  int numInitialParticles = initDelays.size();
  if(numInitialParticles > 0) {
    data = initDelays[0];
    for(int i = 1; i < numInitialParticles; i++) {
      Particle* p = myPlasma->get();
      *p << initDelays[i];
      pstack.putTail(p);
    }
  }
  return numInitialParticles;
}
        // Load up with data
void FixParticle :: operator << (int i) {data = double(i);}
void FixParticle :: operator << (double f) {data = f;}
void FixParticle :: operator << (const Complex& c) {data = double(abs(c));}
void FixParticle :: operator << (const Fix& x) {data = x;}
void FixParticle :: operator << (const Envelope&) {
	Error::abortRun ("Attempt to load a Message into Fix Particle");
}


// Error catcher for attempts to retrieve a Message from a different
// type of particle

class Envelope;

void Particle::accessMessage(Envelope &) const {
	Error::abortRun ("Attempt to getMessage from a non-Message Particle");
}

void Particle::getMessage(Envelope & p) {
	Particle::accessMessage(p);
}

void Particle :: operator << (const Envelope&) {
	Error::abortRun ("Attempt to load a Message into non-Message Particle");
}

