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

// return true if types are equal; return false and print error if not.
int Particle :: compareType (const Particle& src) const {
	if (typesEqual(src)) return 1;
	StringList msg = "Attempt to copy ";
	msg += src.readType();
	msg += " Particle to ";
	msg += readType();
	msg += " Particle";
	Error::abortRun (msg);
	return 0;
}

extern const DataType ANYTYPE = "ANYTYPE";

	///////////////////////////////////////
	// class IntSample
	///////////////////////////////////////

extern const DataType INT = "INT";

Particle* IntSample :: clone () {
	Particle* p = intPlasma.get();
	((IntSample*)p)->data = data;
	return p;
}

void IntSample :: die () { intPlasma.put(this);}

Particle* IntSample :: useNew () { LOG_NEW; return new IntSample;}

Particle& IntSample :: operator = (const Particle& p)
{
	if(compareType(p)) {
		// Types are compatible, so we can copy
		data = ((const IntSample*)&p)->data;
	}
	return *this;
}

int IntSample :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == int(p);
}

DataType IntSample :: readType() const {return INT;}
IntSample :: operator int () const {return data;}
IntSample :: operator double () const {return double(data);}
IntSample :: operator float () const {return float(data);}
IntSample :: operator Complex () const {return Complex(data);}

StringList IntSample :: print () const { return StringList(data);}

	// Wash the Particle
void IntSample :: initialize() {data=0;}

	// Load up with data
void IntSample :: operator << (int i) {data=i;}
void IntSample :: operator << (double f) {data=int(f);}
void IntSample :: operator << (const Complex& c) {data=int(abs(c));}


	////////////////////////////////////////
	// class FloatSample
	////////////////////////////////////////

extern const DataType FLOAT = "FLOAT";

Particle* FloatSample :: useNew () { LOG_NEW; return new FloatSample;}

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
		data = ((const FloatSample*)&p)->data;
	}
	return *this;
}

int FloatSample :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	return data == double(p);
}



DataType FloatSample :: readType() const {return FLOAT;}

        // Cast to an int, double, and Complex
FloatSample :: operator int () const {return int(data);}
FloatSample :: operator double () const {return data;}
FloatSample :: operator float () const {return float(data);}
FloatSample :: operator Complex () const {return Complex(data);}

StringList FloatSample :: print () const { return StringList(data);}

 
        // Initialize the Particle
void FloatSample :: initialize() {data=0.0;}
 
        // Load up with data
void FloatSample :: operator << (int i) {data=i;}
void FloatSample :: operator << (double f) {data=f;}
void FloatSample :: operator << (const Complex& c) {data=abs(c);}

        ////////////////////////////////////////
        // class ComplexSample
        ////////////////////////////////////////

extern const DataType COMPLEX = "COMPLEX";

ComplexSample :: ComplexSample(double f) {data= f;}
ComplexSample :: ComplexSample(int i) {data = double(i);}
ComplexSample :: ComplexSample() {data=0.0;}

Particle* ComplexSample :: useNew () { LOG_NEW; return new ComplexSample;}

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
		const ComplexSample& cs = *(const ComplexSample*)&p;
		data = cs.data;
	}
        return *this;
}

int ComplexSample :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Complex pdata = Complex(data);
	return data.real() == pdata.real() &&
		data.imag() == pdata.imag();
}


DataType ComplexSample :: readType() const {return COMPLEX;}
 
        // Cast to an int, double, Complex
	// when casting to a real, we use the magnitude

ComplexSample :: operator int () const {return int(abs(data));}
ComplexSample :: operator double () const {return abs(data);}
ComplexSample :: operator float () const {return float(abs(data));}
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
void ComplexSample :: operator << (double f) {data=Complex(f);}
void ComplexSample :: operator << (const Complex& c) {data=c;}


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
		DataType dt = p->head->readType();
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

// Error catcher for attempts to retrieve a Packet from a different
// type of particle

class Packet;

void Particle::accessPacket(Packet &) const {
	Error::abortRun ("Attempt to getPacket from a non-packet Particle");
}

void Particle::getPacket(Packet & p) {
	Particle::accessPacket(p);
}

void Particle::operator<<(const Packet&) {
	Error::abortRun ("Attempt to load a Packet into non-packet Particle");
}

