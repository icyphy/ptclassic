#ifndef _particle_h
#define _particle_h 1

#ifdef __GNUG__
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

 Programmer:  E. A. Lee and D. G. Messerschmitt, J. Buck
 Date of creation: 1/17/90
 Revisions:


 Particles are the envelope that carry data between Stars
***************************************************************/

	/////////////////////////////////
	// class Particle
	/////////////////////////////////

class Envelope;

// Particle is a virtual base class -- you can't declare a Particle.
class Particle
{
public:
	// Identify what type of Particle this is
	virtual DataType type() const = 0;

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

	// compare two particles
	virtual int operator == (const Particle&) = 0;

	// clone the Particle, and remove clone
	virtual Particle* clone() const = 0;
	virtual Particle* useNew() const = 0;
	virtual void die() = 0;

	// Message interface: these funcs return errors unless
	// redefined
	virtual void getMessage (Envelope&);
	virtual void accessMessage (Envelope&) const;
	virtual void operator << (const Envelope&);

protected:
	int typesEqual(const Particle& p) const {
		return (type() == p.type());
	}

	// Before copying Particles, always compare their types
	// Otherwise the user could always copy Particles of
	//  incompatible types between an input and output PortHole

	int compareType(const Particle& p) const;

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
* myport%0 = StringParticle("hello") to use other types.
***************************************************************/

	/////////////////////////////////////
	// class IntParticle
	/////////////////////////////////////

class IntParticle : public Particle
{
public:
	// Tell this world this Particle contains an int
	DataType type() const;

	// Cast to an int, double, and Complex
	operator int () const;
	operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;

	// Initialize
	IntParticle(int i) { data = i;}
	IntParticle() {data=0;}

	// Wash the Particle
	void initialize();

	// Load up with data
	void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);

	// Copy the Particle
	Particle& operator = (const Particle&);

	// compare particles
	int operator == (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
	int data;
};

        /////////////////////////////////////
        // class FloatParticle
        /////////////////////////////////////
 
class FloatParticle : public Particle
{
public:
        // Tell this world this Particle contains an double
        DataType type() const;
 
        // Cast to an int, double, and Complex
        operator int () const;
        operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;
 
        // Initialize
        FloatParticle(double f) {data=f;}
        FloatParticle() {data=0.0;}
 
        // Initialize the Particle
        void initialize();
 
        // Load up with data
        void operator << (int i);
        void operator << (double f);
        void operator << (const Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// compare particles
	int operator == (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone() const;
	Particle* useNew() const;
	void die();

private:
        double data;
};

        /////////////////////////////////////
        // class ComplexParticle
        /////////////////////////////////////
 
class ComplexParticle : public Particle
{
public:
        // Tell this world this Particle contains a Complex
        DataType type() const;
 
        // Cast to an int, double, Complex
        operator int () const;
        operator double () const;
	operator float () const;
	operator Complex () const;

	StringList print() const;
 
        // Initialize
        ComplexParticle(const Complex& c) : data(c) {}
	ComplexParticle(double f);
	ComplexParticle(int i);
        ComplexParticle();

        // Initialize the Particle
        void initialize();

        // Load up with data
        void operator << (int i);
        void operator << (double f);
	void operator << (const Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// compare particles
	int operator == (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone() const;
	Particle* useNew() const;
	void die();

 private:
        Complex data;
};

// inline functions to handle other types correctly

inline void operator << (Particle& p, float d) { p << double(d);}

#endif
