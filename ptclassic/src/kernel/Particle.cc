/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:

 Definition of some methods in Particle.h

**************************************************************************/

#include "Particle.h"
#include "Plasma.h"
#include "Error.h"
#include <builtin.h>		// for gnu form(...) function

// Here are the plasmas!
// Create instances of each particle and plasma

static IntSample iproto;
static FloatSample fproto;
static ComplexSample cproto;

Plasma* Plasma :: plasmaList = 0;
static Plasma intPlasma(iproto);
static Plasma floatPlasma(fproto);
static Plasma complexPlasma(cproto);

	///////////////////////////////////////
	// class Particle
	///////////////////////////////////////

void Particle :: badCopy (const Particle& src) const {
	StringList msg = "Attempt to copy ";
	msg += src.readType();
	msg += " Particle to ";
	msg += readType();
	msg += " Particle";
	Error::abortRun (msg);
}

extern const dataType ANYTYPE = "ANYTYPE";

	///////////////////////////////////////
	// class IntSample
	///////////////////////////////////////

extern const dataType INT = "INT";

Particle* IntSample :: clone () {
	Particle* p = intPlasma.get();
	((IntSample*)p)->data = data;
	return p;
}

void IntSample :: die () { intPlasma.put(this);}

Particle* IntSample :: useNew () { return new IntSample;}

Particle& IntSample :: operator = (const Particle& p)
{
	if(compareType(p)) {
		// Types are compatible, so we can copy
		data = ((IntSample*)&p)->data;
	}
	return *this;
}

dataType IntSample :: readType() const {return INT;}
IntSample :: operator int () const {return data;}
IntSample :: operator float () const {return float(data);}
IntSample :: operator Complex () const {return Complex(data);}

StringList IntSample :: print () const { return StringList(data);}

	// Wash the Particle
void IntSample :: initialize() {data=0;}

	// Load up with data
void IntSample :: operator << (int i) {data=i;}
void IntSample :: operator << (float f) {data=int(f);}
void IntSample :: operator << (Complex& c) {data=int(abs(c));}


	////////////////////////////////////////
	// class FloatSample
	////////////////////////////////////////

extern const dataType FLOAT = "FLOAT";

Particle* FloatSample :: useNew () { return new FloatSample;}

Particle* FloatSample :: clone () { 
	Particle* p = floatPlasma.get();
	((FloatSample*)p)->data = data;
	return p;
}


void FloatSample :: die () { floatPlasma.put(this);}

Particle& FloatSample :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
		data = ((FloatSample*)&p)->data;
	}
	return *this;
}


dataType FloatSample :: readType() const {return FLOAT;}

        // Cast to an int, float, and Complex
FloatSample :: operator int () const {return int(data);}
FloatSample :: operator float () const {return data;}
FloatSample :: operator Complex () const {return Complex(data);}

StringList FloatSample :: print () const { return StringList(data);}

 
        // Initialize the Particle
void FloatSample :: initialize() {data=0.0;}
 
        // Load up with data
void FloatSample :: operator << (int i) {data=i;}
void FloatSample :: operator << (float f) {data=f;}
void FloatSample :: operator << (Complex& c) {data=abs(c);}

        ////////////////////////////////////////
        // class ComplexSample
        ////////////////////////////////////////

extern const dataType COMPLEX = "COMPLEX";

Particle* ComplexSample :: useNew () { return new ComplexSample;}

Particle* ComplexSample :: clone () {
	Particle* p = complexPlasma.get();
	((ComplexSample*)p)->data = data;
	return p;
}

void ComplexSample :: die () { complexPlasma.put(this);}

Particle& ComplexSample :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
		data = ((ComplexSample*)&p)->data;
	}
        return *this;
}

dataType ComplexSample :: readType() const {return COMPLEX;}
 
        // Cast to an int, float, Complex
	// when casting to a real, we use the magnitude

ComplexSample :: operator int () const {return int(abs(data));}
ComplexSample :: operator float () const {return abs(data);}
ComplexSample :: operator Complex () const {return data;}

StringList ComplexSample :: print () const {
	StringList out = "(";
	out += data.real();
	out += ",";
	out += data.imag();
	out += ")";
	return out;
}
 
        // Initialize the Particle
void ComplexSample :: initialize() {data=0.0;}

        // Load up with data
void ComplexSample :: operator << (int i) {data=Complex(i);}
void ComplexSample :: operator << (float f) {data=Complex(f);}
void ComplexSample :: operator << (Complex& c) {data=c;}


// ParticleStack methods
// Destructor -- deletes all Particles EXCEPT the last one
// we don't delete the last one because it's the "reference"
// particle (the first one); it normally isn't a dynamically
// created object.

ParticleStack :: ~ParticleStack () {
	if (!head) return;
	Particle* p;
	while (head->link) {
		p = head;
		head = head->link;
		delete p;
	}
}

// freeup -- returns all Particles to their Plasma (including
// the last one)
void ParticleStack :: freeup () {
	Particle* p;
	while (head) {
		p = head;
		head = head->link;
		p->die();
	}
}

Plasma* Plasma :: getPlasma(dataType t)
{
	Plasma* p = plasmaList;

	while (p) {
		dataType dt = p->head->readType();
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

