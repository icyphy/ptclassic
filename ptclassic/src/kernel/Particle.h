#ifndef _particle_h
#define _particle_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif

#include "StringList.h"
#include "DataStruct.h"
#include "dataType.h"
// include all of the Complex class except the stream functions
#include "ComplexSubset.h"

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 1/17/89
 Revisions:


 Particles are the envelope that carry data between Stars
***************************************************************/

	/////////////////////////////////
	// class Particle
	/////////////////////////////////

class Packet;

// Particle is a virtual base class -- you can't declare a Particle.
class Particle
{
public:
	// Identify what type of Particle this is
	virtual DataType readType() const = 0;

	// Cast this Particle to an int, double, Complex
	virtual operator int () const = 0;
	virtual operator float () const = 0;
	virtual operator double () const = 0;
	virtual operator Complex () const = 0;

	// print Particle
	virtual StringList print () const = 0;

	// Zero or initialize this Particle
	virtual void initialize() = 0;

	// Load the Particle with data
	virtual void operator << (int) = 0;
	virtual void operator << (double) = 0;
	virtual void operator << (const Complex&) = 0;

	// Copy a Particle -- since Stars must be able to
	// assign Particles in type-independent fashion,
	// the compiler can't handle this
	virtual Particle& operator = (const Particle&) = 0;

	// clone the Particle, and remove clone
	virtual Particle* clone() = 0;
	virtual Particle* useNew() = 0;
	virtual void die() = 0;

	// packet interface: these funcs return errors unless
	// redefined
	virtual void getPacket (Packet&);
	virtual void operator << (const Packet&);

protected:
	// Before copying Particles, always compare their types
	// Otherwise the user could always copy Particles of
	//  incompatible types between an input and output PortHole
	// we compare function pointers to be fast.
	int compareType(const Particle& p) const {
		if (readType() != p.readType()) {
			badCopy(p);
			return 0;
		}
		return 1;
	}
	void badCopy(const Particle& p) const;
	Particle* link;

	friend class ParticleStack;
};

/***************************************************************
* Types of Particles
*
* Different Particle types carry different types of data
*
* To add a new Particle type, define one analogously to the ones
* below.
* PROBLEM:
* we still have built-in preferences for certain datatypes;
* you'd need to say something like
*
* myport%0 = StringSample("hello") to use other types.
***************************************************************/

	/////////////////////////////////////
	// class IntSample
	/////////////////////////////////////

class IntSample : public Particle
{
public:
	// Tell this world this Particle contains an int
	DataType readType() const;

	// Cast to an int, double, and Complex
	operator int () const;
	operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;

	// Initialize
	IntSample(int i) { data = i;}
	IntSample() {data=0;}

	// Wash the Particle
	void initialize();

	// Load up with data
	void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);

	// Copy the Particle
	Particle& operator = (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone();
	Particle* useNew();
	void die();

private:
	int data;
};

        /////////////////////////////////////
        // class FloatSample
        /////////////////////////////////////
 
class FloatSample : public Particle
{
public:
        // Tell this world this Particle contains an double
        DataType readType() const;
 
        // Cast to an int, double, and Complex
        operator int () const;
        operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;
 
        // Initialize
        FloatSample(double f) {data=f;}
        FloatSample() {data=0.0;}
 
        // Initialize the Particle
        void initialize();
 
        // Load up with data
        void operator << (int i);
        void operator << (double f);
        void operator << (const Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone();
	Particle* useNew();
	void die();

private:
        double data;
};

        /////////////////////////////////////
        // class ComplexSample
        /////////////////////////////////////
 
class ComplexSample : public Particle
{
public:
        // Tell this world this Particle contains a Complex
        DataType readType() const;
 
        // Cast to an int, double, Complex
        operator int () const;
        operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;
 
        // Initialize
        ComplexSample(const Complex& c) {data=c;}
	ComplexSample(double f) {data= f;}
	ComplexSample(int i) {data = double(i);}
        ComplexSample() {data=0.0;}

        // Initialize the Particle
        void initialize();

        // Load up with data
        void operator << (int i);
        void operator << (double f);
	void operator << (const Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone();
	Particle* useNew();
	void die();

 private:
        Complex data;
};

// inline functions to handle other types correctly

inline void operator << (Particle& p, float d) { p << double(d);}

#endif
