static const char file_id[] = "Particle.cc";
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

#ifdef __GNUG__
#pragma implementation
#pragma implementation "ParticleStack.h"
#pragma implementation "Plasma.h"
#endif

#include "Particle.h"
#include "Plasma.h"
#include "Error.h"

// Here are the plasmas!
// Create instances of each particle and plasma

static IntParticle iproto;
static FloatParticle fproto;
static ComplexParticle cproto;

Plasma* Plasma :: plasmaList = 0;
static Plasma intPlasma(iproto);
static Plasma floatPlasma(fproto);
static Plasma complexPlasma(cproto);

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

	///////////////////////////////////////
	// class IntParticle
	///////////////////////////////////////

extern const DataType INT = "INT";

Particle* IntParticle :: clone () {
	Particle* p = intPlasma.get();
	((IntParticle*)p)->data = data;
	return p;
}

void IntParticle :: die () { intPlasma.put(this);}

Particle* IntParticle :: useNew () { LOG_NEW; return new IntParticle;}

Particle& IntParticle :: operator = (const Particle& p)
{
	if(compareType(p)) {
		// Types are compatible, so we can copy
		data = ((const IntParticle*)&p)->data;
	}
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

StringList IntParticle :: print () const { return StringList(data);}

	// Wash the Particle
void IntParticle :: initialize() {data=0;}

	// Load up with data
void IntParticle :: operator << (int i) {data=i;}
void IntParticle :: operator << (double f) {data=int(f);}
void IntParticle :: operator << (const Complex& c) {data=int(abs(c));}


	////////////////////////////////////////
	// class FloatParticle
	////////////////////////////////////////

extern const DataType FLOAT = "FLOAT";

Particle* FloatParticle :: useNew () { LOG_NEW; return new FloatParticle;}

Particle* FloatParticle :: clone () { 
	Particle* p = floatPlasma.get();
	((FloatParticle*)p)->data = data;
	return p;
}


void FloatParticle :: die () { floatPlasma.put(this);}

Particle& FloatParticle :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
		data = ((const FloatParticle*)&p)->data;
	}
	return *this;
}

int FloatParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == double(p);
}



DataType FloatParticle :: type() const {return FLOAT;}

        // Cast to an int, double, and Complex
FloatParticle :: operator int () const {return int(data);}
FloatParticle :: operator double () const {return data;}
FloatParticle :: operator float () const {return float(data);}
FloatParticle :: operator Complex () const {return Complex(data);}

StringList FloatParticle :: print () const { return StringList(data);}

 
        // Initialize the Particle
void FloatParticle :: initialize() {data=0.0;}
 
        // Load up with data
void FloatParticle :: operator << (int i) {data=i;}
void FloatParticle :: operator << (double f) {data=f;}
void FloatParticle :: operator << (const Complex& c) {data=abs(c);}

        ////////////////////////////////////////
        // class ComplexParticle
        ////////////////////////////////////////

extern const DataType COMPLEX = "COMPLEX";

ComplexParticle :: ComplexParticle(double f) {data= f;}
ComplexParticle :: ComplexParticle(int i) {data = double(i);}
ComplexParticle :: ComplexParticle() {data=0.0;}

Particle* ComplexParticle :: useNew () { LOG_NEW; return new ComplexParticle;}

Particle* ComplexParticle :: clone () {
	Particle* p = complexPlasma.get();
	((ComplexParticle*)p)->data = data;
	return p;
}

void ComplexParticle :: die () { complexPlasma.put(this);}

Particle& ComplexParticle :: operator = (const Particle& p)
{
        if(compareType(p)) {
                // Types are compatible, so we can copy
		const ComplexParticle& cs = *(const ComplexParticle*)&p;
		data = cs.data;
	}
        return *this;
}

int ComplexParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Complex pdata = Complex(data);
	return data.real() == pdata.real() &&
		data.imag() == pdata.imag();
}


DataType ComplexParticle :: type() const {return COMPLEX;}
 
        // Cast to an int, double, Complex
	// when casting to a real, we use the magnitude

ComplexParticle :: operator int () const {return int(abs(data));}
ComplexParticle :: operator double () const {return abs(data);}
ComplexParticle :: operator float () const {return float(abs(data));}
ComplexParticle :: operator Complex () const {return data;}

StringList ComplexParticle :: print () const {
	StringList out = "(";
	out += data.real();
	out += ",";
	out += data.imag();
	out += ")";
	return out;
}
 
        // Initialize the Particle
void ComplexParticle :: initialize() {data=0.0;}

        // Load up with data
void ComplexParticle :: operator << (int i) {data=Complex(i);}
void ComplexParticle :: operator << (double f) {data=Complex(f);}
void ComplexParticle :: operator << (const Complex& c) {data=c;}


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
		LOG_DEL; delete p;
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

Plasma* Plasma :: getPlasma(DataType t)
{
	Plasma* p = plasmaList;

	while (p) {
		DataType dt = p->head->type();
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

// Error catcher for attempts to retrieve a Message from a different
// type of particle

class Envelope;

void Particle::accessMessage(Envelope &) const {
	Error::abortRun ("Attempt to getMessage from a non-Message Particle");
}

void Particle::getMessage(Envelope & p) {
	Particle::accessMessage(p);
}

void Particle::operator<<(const Envelope&) {
	Error::abortRun ("Attempt to load a Message into non-Message Particle");
}

