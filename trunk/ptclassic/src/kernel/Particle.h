#ifndef _particle_h
#define _particle_h 1

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

// Particle is a virtual base class -- you can't declare a Particle.
class Particle
{
public:
	// Identify what type of Particle this is
	virtual dataType readType() const = 0;

	// Cast this Particle to an int, float, Complex
	virtual operator int () const = 0;
	virtual operator float () const = 0;
	virtual operator Complex () const = 0;

	// print Particle
	virtual char* print () const = 0;

	// Zero or initialize this Particle
	virtual void initialize() = 0;

	// Load the Particle with data
	virtual void operator << (int) = 0;
	virtual void operator << (float) = 0;
	virtual void operator << (Complex&) = 0;

	// Copy a Particle -- since Stars must be able to
	// assign Particles in type-independent fashion,
	// the compiler can't handle this
	virtual Particle& operator = (const Particle&) = 0;

	// clone the Particle, and remove clone
	virtual Particle* clone() = 0;
	virtual Particle* useNew() = 0;
	virtual void die() = 0;

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

	friend class Plasma;
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
	dataType readType() const;

	// Cast to an int, float, and Complex
	operator int () const;
	operator float () const;
	operator Complex () const;

	char* print() const;

	// Initialize
	IntSample(int i) { data = i;}
	IntSample() {data=0;}

	// Wash the Particle
	void initialize();

	// Load up with data
	void operator << (int i);
	void operator << (float f);
	void operator << (Complex& c);

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
        // Tell this world this Particle contains an float
        dataType readType() const;
 
        // Cast to an int, float, and Complex
        operator int () const;
        operator float () const;
	operator Complex () const;

	char* print() const;
 
        // Initialize
        FloatSample(float f) {data=f;}
        FloatSample() {data=0.0;}
 
        // Initialize the Particle
        void initialize();
 
        // Load up with data
        void operator << (int i);
        void operator << (float f);
        void operator << (Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone();
	Particle* useNew();
	void die();

private:
        float data;
};

        /////////////////////////////////////
        // class ComplexSample
        /////////////////////////////////////
 
class ComplexSample : public Particle
{
public:
        // Tell this world this Particle contains a Complex
        dataType readType() const;
 
        // Cast to an int, float, Complex
        operator int () const;
        operator float () const;
	operator Complex () const;

	char* print() const;
 
        // Initialize
        ComplexSample(const Complex& c) {data=c;}
	ComplexSample(float f) {data=f;}
	ComplexSample(int i) {data = (float)i;}
        ComplexSample() {data=0.0;}

        // Initialize the Particle
        void initialize();

        // Load up with data
        void operator << (int i);
        void operator << (float f);
	void operator << (Complex& c);

        // Copy the Particle
        Particle& operator = (const Particle&);

	// clone the Particle, and remove clone
	Particle* clone();
	Particle* useNew();
	void die();

 private:
        Complex data;
};

/****************************************************************
* Plasma
*
* A Plasma is a collection of currently unused Particles
*
* Particles no longer needed are added to the Plasma; any
* OutPortHoles needing a Particle gets it from the Plasma
*
* There is precisely one instance of Plasma for each
* Particle type needed -- this common pool makes more
* efficient use of memory.  The static member plasmaList
* is a list of all Plasmas.
****************************************************************/

	////////////////////////////////////////
	// class Plasma
	////////////////////////////////////////

class Plasma {
public:
	// Put a Particle into the Plasma
	void put(Particle* p) {p->link = head; head = p;}

	// Get a Particle from the Plasma, creating a
	// new one if necessary.  Never give away last Particle
	Particle* get() { 
		if (head->link) {
			Particle* p = head;
			head = p->link;
			p->initialize();
			return p;
		}
		return head->useNew();
	}
	dataType type() { return head->readType();}

// constructor -- all objs built are added to the static linked list.
	Plasma(Particle& p) : head(&p), nextPlasma(plasmaList) {
		plasmaList = this;
	}
// destructor
	~Plasma();

	static Plasma* getPlasma (dataType t);
private:
	static Plasma* plasmaList;
	Particle* head;
	Plasma* nextPlasma;

};
#endif
